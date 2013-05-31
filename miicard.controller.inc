<?php

/**
 * @file
 * Controller class for miiCards.
 *
 * This extends the DrupalDefaultEntityController class, adding required
 * special handling for miiCard objects.
 */

class MiiCardController extends DrupalDefaultEntityController {
  /**
   * Override attachLoad.
   */
  protected function attachLoad(&$queried_cards, $revision_id = FALSE) {
    // The data field contains a serialised MiiCardUserProfile object.
    foreach ($queried_cards as $key => $record) {
      $queried_cards[$key]->data         = unserialize ($record->data);
      $queried_cards[$key]->access_token = unserialize ($record->access_token);
    }

    // Call the default attachLoad() method.
    parent::attachLoad ($queried_cards, $revision_id);
  }
}
