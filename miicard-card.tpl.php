<?php

/**
 * @file
 * Default theme implementation to present the miiCard profile for the user
 *
 * Available variables:
 * - $username
 *      The miiCard username of the member.
 * - $salutation
 *      The salutation of the member (e.g. 'Mr', 'Mrs' etc).
 * - $first_name
 *      The first name of the member
 * - $middle_name
 *      The middle name of the member, if known.
 * - $last_name
 *      The last name of the member.
 * - $previous_first_name
 *      The previous first name of the member, if known.
 * - $previous_middle_name
 *      The previous middle name of the member, if known.
 * - $previous_last_name
 *      The previous last name of the member, if known.
 * - $last_verified
 *      The UNIX timestamp representing the date the user's identity was last verified.
 * - $profile_url
 *      The URL to the member's public miiCard profile page.
 * - $profile_short_url
 *      The short URL to the member's public miiCard profile page.
 * - $card_image_url
 *      The URL to the user's miiCard card image, as shown on their public profile page.
 * - $emails
 *      An array of emails associated with the member.
 * - $identities
 *      An array of themmed alternative identities for the member.
 * - $phone_numbers
 *      An array of themmed phone numbers.
 * - $postal_addresses
 *      An array of themmed postal address.
 * - $web_properties
 *      An array of string.
 * - $identity_assured
 *      Indicates whether the member has met the level of identity assurance required by your application.
 * - $has_public_profile
 *      Indicates whether the user has published their public profile.
 * - $date_of_birth
 *      The miiCard user's date of birth
 */
?>
<div class="miicard-profile">
  <?php if ($miicard): ?>
    <h3><?php print "{$first_name} {$last_name}"; ?></h3>
    <?php if ($card_image_url && $has_public_profile): ?>
      <a href="<?php print $miicard_url; ?>" target="_blank">
        <img class="miicard-image" alt="<?php print t('miiCard image'); ?>" src="<?php print $card_image_url; ?>"/>
      </a>
    <?php endif; ?>
    <?php if ($identity_assured): ?>
      <p
        class="assured"><?php print t('This user has verified their identity by attaching a miiCard to their account. <a href="@miicard_info_path" target="_blank">What is miiCard?</a>', array('@miicard_info_path' => miicard_url("http://www.miicard.com/how-it-works"))); ?></p>
    <?php else: ?>
      <p class="not-assured"><?php print t("Identity NOT assured!"); ?></p>
    <?php endif; ?>

    <dl>
      <dt><?php print t('Last verified:') ?></dt>
      <dd>
        <?php if ($last_verified): ?>
          <?php print date('j F Y, g:ia', intval($last_verified)); ?>
        <?php else: ?>
          <?php print t('Unknown'); ?>
        <?php endif; ?>
      </dd>
      <?php if (!empty($emails)): ?>
        <dt><?php print t('Email address:'); ?></dt>
        <dd>
          <?php foreach ($emails as $address): ?>
            <?php print $address; ?>
          <?php endforeach; ?>
        </dd>
      <?php endif; ?>
      <?php if (!empty($postal_addresses)): ?>
        <dt><?php print t('Postal addresses'); ?></dt>
        <dd>
          <?php foreach ($postal_addresses as $address): ?>
            <?php print $address; ?>
          <?php endforeach; ?>
        </dd>
      <?php endif; ?>
      <?php if (!empty($phone_numbers)): ?>
        <dt><?php print t('Phone number:'); ?></dt>
        <dd>
          <?php foreach ($phone_numbers as $number): ?>
            <?php print $number; ?>
          <?php endforeach; ?>
        </dd>
      <?php endif; ?>
      <?php if (!empty($web_properties)): ?>
        <dt><?php print t('Web properties:'); ?></dt>
        <dd>
          <?php foreach ($web_properties as $property): ?>
            <?php print $property; ?>
          <?php endforeach; ?>
        </dd>
      <?php endif; ?>
    </dl>

    <?php if ($profile_url && $has_public_profile): ?>
      <p><a href="<?php print $miicard_url; ?>" target="_blank">
          <?php print t('Visit miiCard Profile'); ?>
        </a></p>
    <?php endif; ?>

    <?php if (!empty($identities)): ?>
      <ul>
        <?php foreach ($identities as $identity): ?>
          <li><?php print $identity; ?></li>
        <?php endforeach; ?>
      </ul>
    <?php endif; ?>
  <?php endif; ?>

</div>
