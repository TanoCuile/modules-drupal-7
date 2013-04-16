<?php
/**
 * @file
 * Set up admin page for miiCard
 */

/**
 * Implements hook_admin_settings().
 */
function miicard_admin_settings() {
  $form = array();

  $form['intro'] = array(
    '#markup' => t("<p>You will need to sign up for a miiCard developer account to obtain a consumer key and secret. You can request access by filling in the sign-up form at <a href='http://www.miicard.com/developers/getting-started'>http://www.miicard.com/developers/getting-started</a>.</p>"),
  );

  $form['miicard_consumer_key'] = array(
    '#type' => 'textfield',
    '#title' => t('miiCard Consumer Key'),
    '#default_value' => variable_get('miicard_consumer_key', ''),
    '#required' => TRUE,
  );

  $form['miicard_consumer_secret'] = array(
    '#type' => 'textfield',
    '#title' => t('miiCard Consumer Secret'),
    '#default_value' => variable_get('miicard_consumer_secret', ''),
    '#required' => TRUE,
  );

  $form['miicard_affiliate_code'] = array(
    '#type' => 'textfield',
    '#title' => t('miiCard Affiliate Code'),
    '#default_value' => variable_get('miicard_affiliate_code', ''),
    '#required' => FALSE,
    '#description' => 'Your miiCard affiliate code, if you are part of the miiCard affiliate programme. When set, URLs to miiCard.com created via the miicard_url function will contain your affiliate code so that new signups resulting from referrals from your site are recorded properly. Find out more at <a href="http://www.miicard.com/developers/affiliate-codes" target="_blank">http://www.miicard.com/developers/affiliate-codes</a>.',
  );

  $form['miicard_add_link'] = array(
    '#type' => 'textfield',
    '#title' => t('Add miiCard link text'),
    '#default_value' => variable_get('miicard_add_link', MIICARD_ADD_LINK),
    '#required' => TRUE,
  );

  $form['miicard_sign_in_link'] = array(
    '#type' => 'textfield',
    '#title' => t('Sign in with miiCard link text'),
    '#default_value' => variable_get('miicard_sign_in_link', MIICARD_SIGN_IN_LINK),
    '#required' => TRUE,
  );

  $form['miicard_remove_link'] = array(
    '#type' => 'textfield',
    '#title' => t('Remove miiCard link text'),
    '#default_value' => variable_get('miicard_remove_link', MIICARD_REMOVE_LINK),
    '#required' => TRUE,
  );

  $form['miicard_allow_create_accounts'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow creation of new accounts via miiCard'),
    '#default_value' => variable_get('miicard_allow_create_accounts', FALSE),
    '#required' => FALSE,
    '#description' => t('If set, a new user signing in with their miiCard will be shown the account creation form. If not set, users will have to create an account as normal, then attach a miiCard to it.'),
  );

  if (variable_get('user_register', USER_REGISTER_VISITORS_ADMINISTRATIVE_APPROVAL) == USER_REGISTER_VISITORS_ADMINISTRATIVE_APPROVAL) {
    $form['miicard_auto_approve_accounts'] = array(
      '#type' => 'checkbox',
      '#title' => t('Automatically approve miiCard-created accounts'),
      '#default_value' => variable_get('miicard_auto_approve_accounts', FALSE),
      '#required' => FALSE,
      '#description' => t('If set, accounts created via miiCard will be automatically approved without requiring administrator intervention but only if they use an email address that has been verified by miiCard.'),
    );
  }

  $form['miicard_highlight_approved_users'] = array(
    '#type' => 'checkbox',
    '#title' => t('Highlight miiCard-verified users'),
    '#default_value' => variable_get('miicard_highlight_approved_users'),
    '#required' => FALSE,
    '#description' => 'Shows a small miiCard glyph next to miiCard-linked usernames on contributions to your site.',
  );

  $period = array(
    0 => t('Never'),
    86400 => t('24 hours'),
    172800 => t('2 days'),
    604800 => t('1 week'),
  );

  $form['miicard_refresh_period'] = array(
    '#type' => 'select',
    '#title' => t('Refresh period'),
    '#description' => t("The refresh period controls a cron job that periodically updates the miiCard identity information for any miiCard-linked member of your site."),
    '#options' => $period,
    '#default_value' => variable_get('miicard_refresh_period', 0),
  );

  if (module_exists('profile')) {
    module_load_include('inc', 'miicard', 'miicard.profile');
  }
  else {
    $form['miicard_enable_profile_module'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use profile module'),
      '#description' => t("If you need to use the legacy Profile module rather than Fields UI, this option will enable the module for you."),
      '#default_value' => 0,
    );
    $form['#submit'][] = 'miicard_admin_settings_form_submit';
  }

  return system_settings_form($form);
}

/**
 * Implements hook _admin_settings_form_submit().
 */
function miicard_admin_settings_form_submit($form, &$form_state) {
  if ($form_state['values']['miicard_enable_profile_module']) {
    module_enable(array('profile'));
    drupal_flush_all_caches();
  }
}
