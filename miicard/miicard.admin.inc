<?php
/**
 * @file
 * Set up admin page for miiCard
 */
/**
 * Implements hook admin_settings().
 * @return Ambigous <The, string>
 */
function miicard_admin_settings() {
  $form = array();

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

  $form['miicard_oauth'] = array(
    '#type' => 'textfield',
    '#title' => t('miiCard OAuth Endpoint'),
    '#default_value' => variable_get('miicard_oauth', MIICARD_OAUTH),
    '#required' => TRUE,
  );

  $form['miicard_api'] = array(
    '#type' => 'textfield',
    '#title' => t('miiCard API Endpoint'),
    '#default_value' => variable_get('miicard_api', MIICARD_API),
    '#required' => TRUE,
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
    '#title' => t('Sign in with miiCard link text'),
    '#default_value' => variable_get('miicard_remove_link', MIICARD_REMOVE_LINK),
    '#required' => TRUE,
  );

  $period = array(
    0 => t('Never'),
    21600 => t('6 hours'),
    43200 => t('12 hours'),
    86400 => t('24 hours'),
    172800 => t('2 days'),
    604800 => t('1 week'),
  );

  $form['miicard_refresh_period'] = array(
    '#type' => 'select',
    '#title' => t('Refresh period'),
    '#description' => t("Refresh users' miiCards on cron"),
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
