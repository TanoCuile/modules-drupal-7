<?php

/**
 * Pass user to miiCard to log in.
 */
function miicard_redirect() {
  $key = variable_get('miicard_consumer_key', '');
  $secret = variable_get('miicard_consumer_secret', '');
  $miicard = new miiCard($key, $secret);
  $token = $miicard->get_request_token();
  
  $_SESSION['miicard_oauth']['token'] = $token;
  $_SESSION['miicard_oauth']['destination'] = $_SERVER['HTTP_REFERER'];

  drupal_goto($miicard->get_authorise_url($token));
}

/**
 * Handler for callback from miiCard log in
 * Register new user and/or associate miiCard to Drupal user
 * Log in user if miiCard associated
 */
function miicard_callback() {
  global $user;

  if (!array_key_exists('miicard_oauth', $_SESSION)) {
    $_SESSION['miicard_oauth'] = array();
  }

  if (empty($_SESSION['miicard_oauth'])) {
    $token_store = array();
    $redirect = 'user/login';
  }
  else {
    $token_store = $_SESSION['miicard_oauth']['token'];
    $redirect = $_SESSION['miicard_oauth']['destination'];
  }
  
  $token = array_key_exists('oauth_token', $_REQUEST) ? $_REQUEST['oauth_token'] : '';
  $verifier = array_key_exists('oauth_verifier', $_REQUEST) ? $_REQUEST['oauth_verifier'] : '';

  if (empty($token) || empty($token_store) || $token != $token_store['oauth_token']) {
    drupal_set_message(t('Failed to login with miiCard'), 'error');
  }
  else {
    $key = variable_get('miicard_consumer_key', '');
    $secret = variable_get('miicard_consumer_secret', '');

    $miicard = new miiCard($key, $secret, $token_store['oauth_token'], $token_store['oauth_token_secret']);
    $token = $miicard->get_access_token($verifier);
    if (empty($token)) {
      drupal_goto($redirect);
    }
    
    $miicard->dictionary_request();
    $account = _miicard_load_user($miicard);
    
    if (user_is_anonymous()) {
      if ($account->uid) {
        // Log in user
        $miicard->uid = $account->uid;
        _miicard_identity_save($miicard);
        
        $form_state['uid'] = $account->uid;
        user_login_submit(array(), $form_state);
      }
      else {
        // Register user
        $username = _miicard_generate_username($miicard);
        if (empty($username)) {
          drupal_set_message(t('Could not register new user, please log in or register first'), 'warning');
          $redirect = 'user';
        }
        else {
          user_external_login_register($username, 'miicard');
          $account = user_external_load($username);
          if ($account) {
            $miicard->uid = $account->uid;
            _miicard_identity_save($miicard);
          }
        }
      }
    }
    else {
      if ($account->uid) {
        if ($account->uid == $user->uid) {
          // Update miiCard
          _miicard_identity_save($miicard);
        }
        else {
          // miiCard already registered to another user.
          // Update miiCard and login as that user
          $miicard->uid = $account->uid;
          _miicard_identity_save($miicard);
          
          $form_state['uid'] = $account->uid;
          user_login_submit(array(), $form_state);          
        }
      }
      else {
        // Add new miicard record
        $miicard->uid = $user->uid;
        if (_miicard_identity_save($miicard)) {
          drupal_set_message(t('miiCard added to your account'));
        }
      }
    }
  }

  drupal_goto($redirect);
}

/**
 * Load Drupal user from miiCard class
 * 
 * @param $miicard
 * The miiCard class after call to API
 */
function _miicard_load_user(&$miicard) {
  $uid = 0;
  
  if ($miicard->uid && !$miicard->error) {
    $uid = $miicard->uid;
  }
  elseif (!$miicard->error) {
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'miicard')
          ->propertyCondition('username', $miicard->username)
          ->range(0, 1)
          ->addMetaData('account', user_load(1));
    $result = $query->execute();

    if (isset($result['miicard'])) {
      $card_ids = array_keys($result['miicard']);
      $card = reset(entity_load('miicard', $card_ids));
      $uid = $card->uid;
    }
  }

  return user_load($uid);
}

/**
 * Generate a username from the card profile in the form username@miicard.com
 *
 * @param $miicard
 * The miiCard class after call to API
 */
function _miicard_generate_username(&$miicard) {
  $name = $miicard->data['Username'] . '@miicard.com';
  
  return $name;
}

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