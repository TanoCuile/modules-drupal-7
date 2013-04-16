<?php
/**
 * @file
 * Page handling
 */

/**
 * Pass user to miiCard to log in.
 */
function miicard_redirect() {
  if (!isset($_SESSION)) {
    drupal_session_start();
  }

  $key = variable_get('miicard_consumer_key', '');
  $secret = variable_get('miicard_consumer_secret', '');
  $miicard = new MiiCard($key, $secret);

  $_SESSION['miicard_oauth'] = array();

  // Record where we got here from
  $_SESSION['miicard_oauth']['destination'] = $_SERVER['HTTP_REFERER'];

  // Start OAuth authentication process
  $miicard->beginAuthorisation(url('user/login/miicard/callback', array('absolute' => TRUE)));
  return;
}

function miicard_register() {
  $miicard = _miicard_get_registration_mode_wrapper();

  $token = $miicard->getAccessToken();
  $tokenSecret = $miicard->getAccessTokenSecret();

  $username = $_SESSION['miicard_oauth']['miicard_username'];
  $email = $_SESSION['miicard_oauth']['miicard_email'];

  if ($token != NULL && $tokenSecret != NULL) {
    $form = drupal_get_form('user_register_form');

    $form['account']['name']['#value'] = empty($form_state['input']['name']) ? $username : $form_state['input']['name'];
    $form['account']['mail']['#parents'] = array('name');
    $form['account']['mail']['#value'] = empty($form_state['input']['mail']) ? $email : $form_state['input']['mail'];
    $form['account']['mail']['#parents'] = array('mail');
  }

  $form['#validate'][] = 'miicard_register_validate';
  $form['#submit'][] = 'miicard_register_submit';

  return $form;
}

/**
 * Handler for callback from miiCard log in
 * Register new user and/or associate miiCard to Drupal user
 * Log in user if miiCard associated
 */
function miicard_callback() {
  global $user;

  if (!isset($_SESSION)) {
    drupal_session_start();
  }

  $key = variable_get('miicard_consumer_key', '');
  $secret = variable_get('miicard_consumer_secret', '');
  $token = isset($_REQUEST['accessToken']) ? $_REQUEST['accessToken'] : NULL;
  $verifier = isset($_REQUEST['accessTokenSecret']) ? $_REQUEST['accessTokenSecret'] : NULL;

  $referrer_code = isset($_REQUEST['$referrerCode']) ? $_REQUEST['$referrerCode'] : NULL;
  $force_claims_picker = isset($_REQUEST['$forceClaimsPicker']) ? $_REQUEST['$forceClaimsPicker'] : NULL;

  // Get redirect URL
  $redirect = isset($_SESSION['miicard_oauth']['destination'])
            ? $_SESSION['miicard_oauth']['destination']
            : url('user/login', array('absolute' => TRUE));

  // Create miiCard object with as much information as we have
  $miicard = new MiiCard($key, $secret);//, $token, $verifier, $referrer_code, $force_claims_picker);

  // If we are in the middle of OAuth callback
  if ($miicard->isAuthorisationCallback()) {

    // Continue with authorisation
    $miicard->handleAuthorisationCallback();

    if ($miicard->isAuthorisationSuccess()) {
      $token = $miicard->getAccessToken();
      $verifier = $miicard->getAccessTokenSecret();

      if (empty($token)) {
        drupal_goto($redirect);
      }

      $profile = $miicard->getUserProfile();
      $account = _miicard_load_user($profile);

      if (user_is_anonymous()) {
        if ($account->uid) {
          // Log in user
          _miicard_identity_save($miicard, $account->uid);

          $form_state['uid'] = $account->uid;

          if (user_is_blocked($account->name))
          {
            drupal_set_message(t('The username %name has not been activated or is blocked.', array('%name' => $account->name)), 'error');
            drupal_goto();
          }
          else
          {
            user_login_submit(array(), $form_state);
          }
        }
        else {
          $_SESSION['miicard_oauth']['access_token'] = $token;
          $_SESSION['miicard_oauth']['access_token_secret'] = $verifier;

          $emails = $profile->getEmailAddresses();

          if ($emails != NULL && count($emails) > 0){
            $email = $emails[0];
            $_SESSION['miicard_oauth']['miicard_email'] = $email->getAddress();
          }
          else {
            $_SESSION['miicard_oauth']['miicard_email'] = null;
          }

          $_SESSION['miicard_oauth']['miicard_username'] = $profile->getUsername();

          drupal_goto('user/login/miicard/register');
        }
        /*
          // Register user
          $username = _miicard_generate_username($profile);
          if (empty($username)) {
            drupal_set_message(t('Could not register new user, please log in or register first'), 'warning');
            $redirect = 'user';
          }
          else {
            user_external_login_register($username, 'miicard');
            $account = user_external_load($username);
            if ($account) {
              _miicard_identity_save($miicard, $account->uid);
            }
          }
        }
        */
      }
      else {
        if ($account->uid) {
          if ($account->uid == $user->uid) {
            // Update miiCard
            _miicard_identity_save($miicard, $account->uid);
          }
          else {
            // miiCard already registered to another user.
            // Update miiCard and login as that user
            _miicard_identity_save($miicard, $account->uid);

            $form_state['uid'] = $account->uid;
            user_login_submit(array(), $form_state);
          }
        }
        else {
          // Add new miicard record
          if (_miicard_identity_save($miicard, $user->uid)) {
            drupal_set_message(t('miiCard added to your account'));
          }
        }
      }
    }
    else {
      // Something has gone wrong, report to user
      drupal_set_message(t('Failed to login with miiCard'), 'error');
    }
  }

  drupal_goto($redirect);
}