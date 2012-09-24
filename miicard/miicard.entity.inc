<?php

/**
 * Controller class for miiCards.
 *
 * This extends the DrupalDefaultEntityController class, adding required
 * special handling for miiCard objects.
 */
class miiCardController extends DrupalDefaultEntityController {

  function attachLoad(&$queried_cards, $revision_id = FALSE) {
    // Unserialize data field.  This should probably be replaced with separate fields
    foreach ($queried_cards as $key => $record) {
      $queried_cards[$key]->data = unserialize($record->data);
      $queried_cards[$key]->access_token = unserialize($record->access_token);
    }

    // Call the default attachLoad() method.
    parent::attachLoad($queried_cards, $revision_id);
  }
}
