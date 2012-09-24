<?php

/**
 * @file
 * miiCard to Fields module mapping.
 */

/**
 * Mapping miiCard fields to Field module fields.
 */
function miicard_map_fields_form() {
  $form = array();
  $form['miicard_user_fields'] = array(
    '#type' => 'fieldset',
    '#title' => t('User field mapping'),
    '#description' => t('Each of your <a href="!url">fields attached to users</a> are listed below. Select the miiCard data you want to import into each field.', array('!url' => url('admin/config/people/accounts/fields'))),
    '#tree' => TRUE,
    '#weight' => 5,
  );

  // Each field type can only map to certain type Facebook properties. Build a
  // list for each type that includes reasonable options.
  $properties = miicard_mapping_options();
  $property_options = array();
  foreach ($properties as $property => $property_info) {
    if (isset($property_info['field_types'])) {
      foreach ($property_info['field_types'] as $field_type) {
        $property_options[$field_type][$property] = $property_info['label'];
      }
    }
  }

  $field_defaults = variable_get('miicard_user_fields', array());
  $instances = field_info_instances('user', 'user');
  foreach ($instances as $field_name => $instance) {
    $field = field_info_field($instance['field_name']);
    if (isset($property_options[$field['type']])) {
      $options = array_merge(array('' => t('- Do not import -')), $property_options[$field['type']]);
      $form['miicard_user_fields'][$field_name] = array(
        '#title' => t($instance['label']),
        '#type' => 'select',
        '#options' => $options,
        '#default_value' => isset($field_defaults[$field_name]) ? $field_defaults[$field_name] : '',
      );
    }
    else {
      $form['miicard_user_fields'][$field_name] = array(
        '#title' => t($instance['label']),
        '#type' => 'form_element',
        '#children' => '<em>' . t('No mappable miiCard properties.') . '</em>',
        '#theme_wrappers' => array('form_element'),
      );
    }
  }
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#weight' => 100,
  );
  
  return $form;
}

/**
 * Submit handler for the map miicard fields form.
 */
function miicard_map_fields_form_submit(&$form, &$form_state) {
  if (isset($form_state['values']['miicard_user_fields'])) {
    variable_set('miicard_user_fields', array_filter($form_state['values']['miicard_user_fields']));
  }
  else {
    variable_set('miicard_user_fields', array());
  }
}

/**
 * Add field info to a Drupal user array ready to save.
 */
function miicard_field_create_user(&$edit, $miicard) {
  $field_map = variable_get('miicard_user_fields', array());
  $field_mapping_options = miicard_mapping_options();
  $instances = field_info_instances('user', 'user');

  foreach ($instances as $field_name => $instance) {
    $user_field = field_info_field($instance['field_name']);
    if (isset($field_map[$field_name])) {
      $field_mapping = $field_mapping_options[$field_map[$field_name]];
      $callback = 'miicard_field_convert_' . $field_mapping['conversion'];
      if ($value = $callback($field_mapping['miicard'], $miicard, $user_field, $instance)) {
        $edit[$field_name][LANGUAGE_NONE][0] = $value;
      }
      else {
        $edit[$field_name][LANGUAGE_NONE][0] = array('value' => NULL);
      }
    }
  }
}


/**
 * miiCard data conversion functions.
 */
function miicard_field_convert_text($miicard_property_name, $miicard, $field, $instance) {
  $value = NULL;
  if (isset($miicard->data[$miicard_property_name])) {
    // Simple string values only, not arrays.
    if (is_string($miicard->data[$miicard_property_name])) {
      $value = $miicard->data[$miicard_property_name];
    }
  }

  return $value ? array('value' => $value) : NULL;
}

/**
 * This function returns all three date formats (date, datetime, and datestamp) 
 * and returns the appropriate data. Both date and datetime use ISO format of
 * YYYY-MM-DDTHH:MM:SS. Datestamp uses a UNIX timestamp.
 */
function miicard_field_convert_date($miicard_property_name, $miicard, $field, $instance) {
  $value = NULL;
  $type = isset($field['type']) ? $field['type'] : 'datetime';
  $date = isset($miicard->data[$miicard_property_name]) ? $miicard->data[$miicard_property_name] : 0;
  if ($date > 0) {
    if ($type == 'profile_date') {
      $d = getdate($date);
      $value = array(
        'value' => array(
        	'year' => $d['year'], 
        	'month' => $d['mon'], 
        	'day' => $d['mday'],
        ),
      );
    }
    else {
      $format = ($type == 'datestamp' ? 'U' : 'Y-m-d\TH:i:s');
      $value = array(
        'value' => date($format, $date),
        'date_type' => $type,
      );
    }
  }

  return $value;
}

function miicard_field_convert_full_name($miicard_property_name, $miicard, $field, $instance) {
  $value = NULL;
  if (isset($miicard->data['FirstName']) && isset($miicard->data['LastName'])) {
    if (is_string($miicard->data['FirstName']) && is_string($miicard->data['LastName'])) {
      $value = $miicard->data['FirstName'] . ' ' . $miicard->data['LastName'];
    }
  }

  return $value ? array('value' => $value) : NULL;
}

function miicard_field_convert_email_primary($miicard_property_name, $miicard, $field, $instance) {
  $value = NULL;
  if (isset($miicard->data[$miicard_property_name])) {
    $value = miicard_collection_primary($miicard->data[$miicard_property_name], 'miicard_convert_email_type');
  }
  
  return $value ? array('value' => $value) : NULL;
}

function miicard_field_convert_email_list($miicard_property_name, $miicard, $field, $instance) {
  $value = NULL;
  $list = array();
  if (isset($miicard->data[$miicard_property_name])) {
    $list = miicard_collection_list($miicard->data[$miicard_property_name], 'miicard_convert_email_type');
  }
  $value = join(', ', $list);
  
  return $value ? array('value' => $value) : NULL;
}

function miicard_field_convert_postal_primary($miicard_property_name, $miicard, $field, $instance) {
  $value = NULL;
  if (isset($miicard->data[$miicard_property_name])) {
    $value = miicard_collection_primary($miicard->data[$miicard_property_name], 'miicard_convert_postal_type');
  }
  
  return $value ? array('value' => $value) : NULL;
}

function miicard_field_convert_postal_list($miicard_property_name, $miicard, $field, $instance) {
  $value = NULL;
  $list = array();
  if (isset($miicard->data[$miicard_property_name])) {
    $list = miicard_collection_list($miicard->data[$miicard_property_name], 'miicard_convert_postal_type');
  }
  $value = join("\n\n", $list);
  
  return $value ? array('value' => $value) : NULL;
}

function miicard_field_convert_identity_list($miicard_property_name, $miicard, $field, $instance) {
  $value = NULL;
  $list = array();
  if (isset($miicard->data[$miicard_property_name])) {
    $list = miicard_collection_list($miicard->data[$miicard_property_name], 'miicard_convert_identity_type');
  }
  $value = theme('item_list', array('items' => $list));
  
  return $list ? array('value' => $value) : NULL;
}

function miicard_field_convert_phone_primary($miicard_property_name, $miicard, $field, $instance) {
  $value = NULL;
  if (isset($miicard->data[$miicard_property_name])) {
    $value = miicard_collection_primary($miicard->data[$miicard_property_name], 'miicard_convert_phone_type');
  }
  
  return $value ? array('value' => $value) : NULL;
}

function miicard_field_convert_phone_list($miicard_property_name, $miicard, $field, $instance) {
  $value = NULL;
  $list = array();
  if (isset($miicard->data[$miicard_property_name])) {
    $list = miicard_collection_list($miicard->data[$miicard_property_name], 'miicard_convert_phone_type');
  }
  $value = join(', ', $list);
  
  return $value ? array('value' => $value) : NULL;
}

function miicard_field_convert_profile_url($miicard_property_name, $miicard, $field, $instance) {
  $value = NULL;
  if (isset($miicard->data[$miicard_property_name]) && isset($miicard->data['HasPublicProfile'])) {
    if ($miicard->data['HasPublicProfile']) {
      $value = miicard_field_convert_text($miicard_property_name, $miicard, $field, $instance);
    }
  }
  
  return $value;
}

function miicard_field_convert_profile_link($miicard_property_name, $miicard, $field, $instance) {
  $value = NULL;
  if ($url = miicard_field_convert_profile_url($miicard_property_name, $miicard, $field, $instance)) {
    $value = l(t('miiCard profile'), $url['value']);
  }
  
  return $value ? array('value' => $value) : NULL;
}

function miicard_field_convert_card_image_link($miicard_property_name, $miicard, $field, $instance) {
  $value = NULL;
  if ($url = miicard_field_convert_text('ProfileUrl', $miicard, $field, $instance)) {
    if ($src = miicard_field_convert_text($miicard_property_name, $miicard, $field, $instance)) {
      $value = l(theme('image', array('path' => $src['value'], 'alt' => t('miiCard profile image'))), $url['value'], array('html' => TRUE));
    }
  }
  
  return $value ? array('value' => $value) : NULL;
}

function miicard_convert_email_type($item) {
  return $item['Address'];
}

function miicard_convert_identity_type($item) {
  return l($item['Source'], $item['ProfileUrl']);
}

function miicard_convert_phone_type($item) {
  return '+' . $item['CountryCode'] . $item['NationalNumber'];
}

function miicard_convert_postal_type($item) {
  $address = array(
    $item['House'],
    $item['Line1'],
    $item['Line2'],
    $item['City'],
    $item['Region'],
    $item['Code'],
    $item['Country'],
  );
  
  return join("\n", $address);
}

function miicard_collection_list($collection, $conversion) {
  $list = array();
  if (is_array($collection)) {
    foreach ($collection as $item) {
      if ($item['Verified']) {
        $list[] = $conversion($item);
      }
    }
  }
  
  return $list;
}

function miicard_collection_primary($collection, $conversion) {
  $value = NULL;
  if (is_array($collection)) {
    foreach ($collection as $item) {
      if ($item['IsPrimary'] && $item['Verified']) {
        $value = $conversion($item);
      }
    }
  }
  
  return $value;
}

function miicard_mapping_options() {
  $options = array(
    'salutation' => array(
      'miicard' => 'Salutation',
      'label' => t('Salutation'),
      'conversion' => 'text',
      'field_types' => array('text', 'textfield'),
    ),
    'full_name' => array(
      'miicard' => 'COMPOUND',
      'label' => t('Full Name'),
      'conversion' => 'full_name',
      'field_types' => array('text', 'textfield'),
    ),
    'first_name' => array(
      'miicard' => 'FirstName',
      'label' => t('First Name'),
      'conversion' => 'text',
      'field_types' => array('text', 'textfield'),
    ),
    'middle_name' => array(
      'miicard' => 'MiddleName',
      'label' => t('Middle Name'),
      'conversion' => 'text',
      'field_types' => array('text', 'textfield'),
    ),
    'last_name' => array(
      'miicard' => 'LastName',
      'label' => t('Last Name'),
      'conversion' => 'text',
      'field_types' => array('text', 'textfield'),
    ),
    'previous_first_name' => array(
      'miicard' => 'PreviousFirstName',
      'label' => t('Previous First Name'),
      'conversion' => 'text',
      'field_types' => array('text', 'textfield'),
    ),
    'previous_middle_name' => array(
      'miicard' => 'PreviousMiddleName',
      'label' => t('Previous Middle Name'),
      'conversion' => 'text',
      'field_types' => array('text', 'textfield'),
    ),
    'previous_last_name' => array(
      'miicard' => 'PreviousLastName',
      'label' => t('Previous Last Name'),
      'conversion' => 'text',
      'field_types' => array('text', 'textfield'),
    ),
    'last_verified' => array(
      'miicard' => 'LastVerified',
      'label' => t('Last Verified'),
      'conversion' => 'date',
      'field_types' => array('text', 'textfield', 'date', 'datetime', 'datestamp'),
    ),
    'profile_url' => array(
      'miicard' => 'ProfileUrl',
      'label' => t('Public Profile URL'),
      'conversion' => 'profile_url',
      'field_types' => array('text', 'textfield'),
    ),
    'profile_link' => array(
      'miicard' => 'ProfileUrl',
      'label' => t('Public Profile Link'),
      'conversion' => 'profile_link',
      'field_types' => array('text_long', 'textarea'),
    ),
    'profile_short_url' => array(
      'miicard' => 'ProfileShortUrl',
      'label' => t('Public Profile Short URL'),
      'conversion' => 'profile_url',
      'field_types' => array('text', 'textfield'),
    ),
    'profile_short_link' => array(
      'miicard' => 'ProfileShortUrl',
      'label' => t('Public Profile Short Link'),
      'conversion' => 'profile_link',
      'field_types' => array('text_long', 'textarea'),
    ),
    'card_image_url' => array(
      'miicard' => 'CardImageUrl',
      'label' => t('Card Image URL'),
      'conversion' => 'text',
      'field_types' => array('text', 'textfield'),
    ),
    'card_image_link' => array(
      'miicard' => 'CardImageUrl',
      'label' => t('Card Image Link'),
      'conversion' => 'card_image_link',
      'field_types' => array('text_long', 'textarea'),
    ),
    'email_address_list' => array(
      'miicard' => 'EmailAddresses',
      'label' => t('E-mail Address List'),
      'conversion' => 'email_list',
      'field_types' => array('text_long', 'textarea', 'list'),
    ),
    'email_address_primary' => array(
      'miicard' => 'EmailAddresses',
      'label' => t('Primary E-mail Address'),
      'conversion' => 'email_primary',
      'field_types' => array('text', 'textfield'),
    ),
    'identities' => array(
      'miicard' => 'Identities',
      'label' => t('Identities'),
      'conversion' => 'identity_list',
      'field_types' => array('text_long', 'textarea'),
    ),
    'phone_number_list' => array(
      'miicard' => 'PhoneNumbers',
      'label' => t('Phone Number List'),
      'conversion' => 'phone_list',
      'field_types' => array('text', 'text_long', 'textfield', 'textarea', 'list'),
    ),
    'phone_number_primary' => array(
      'miicard' => 'PhoneNumbers',
      'label' => t('Primary Phone Number'),
      'conversion' => 'phone_primary',
      'field_types' => array('text', 'textfield'),
    ),
    'postal_address_list' => array(
      'miicard' => 'PostalAddresses',
      'label' => t('Postal Address List'),
      'conversion' => 'postal_list',
      'field_types' => array('text_long', 'textarea'),
    ),
    'postal_address_primary' => array(
      'miicard' => 'PostalAddresses',
      'label' => t('Primary Postal Address'),
      'conversion' => 'postal_primary',
      'field_types' => array('text_long', 'textarea'),
    ),
  );
  ksort($options);

  return $options;
}
