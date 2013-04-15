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

  // Record where we got here from
  $_SESSION['miicard_oauth']['destination'] = $_SERVER['HTTP_REFERER'];

  // Start OAuth authentication process
  $miicard->beginAuthorisation(url('user/login/miicard/callback', array('absolute' => TRUE)));
  return;
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
  if ($miicard != NULL && $miicard->isAuthorisationCallback()) {

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
          user_login_submit(array(), $form_state);
        }
        else {
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
            $redirect = "user/"; // Redirect to new profile page
            drupal_set_message(t('miiCard already associated. Switched to user ') . $account->name);
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

/**
 * Load Drupal user from miiCard class
 *
 * @param $profile
 * The miiCard profile of the current user
 */
function _miicard_load_user(&$profile) {
  $uid = 0;
  // No way to persist $uid in miiCard API so set it to zero
  // for now but leave code so we can change this in future.
//  $uid = $profile->getIdentity()->getUserId();

  if ($uid == 0) {
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'miicard')
          ->propertyCondition('username', $profile->getUsername())
          ->range(0, 1)
          ->addMetaData('account', user_load(1));
    $result = $query->execute();

    if (isset($result['miicard'])) {
      $card_ids = array_keys($result['miicard']);
      $entity = entity_load('miicard', $card_ids);
      $card = reset($entity);
      $uid = $card->uid;
    }
  }

  return user_load($uid);
}

/**
 * Generate a username from the card profile in the form username@miicard.com
 *
 * @param $profile
 * The miiCard profile of the current user
 */
function _miicard_generate_username(&$profile) {
  $name = $profile->getUsername();
  return $name . '@miicard.com';
}

/**
 * Remove miiCard identity associated with user
 */
function miicard_remove() {
  global $user;

  _miicard_identity_delete($user);
  $miicard = _miicard_identity_load($user);
  if (empty($miicard)) {
    drupal_set_message(t('miiCard identity removed'));
  }
  else {
    drupal_set_message(t('Could not remove miiCard identity!'), 'error');
  }

  drupal_goto('user/' . $user->uid . '/edit');
}