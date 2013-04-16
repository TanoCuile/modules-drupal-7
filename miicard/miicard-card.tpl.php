<?php

/**
 * @file
 * Default theme implementation to present the miiCard profile for the user
 *
 * Available variables:
 * - $miicard: Array returned by the miiCard API
 */
?>
<div class="miicard-profile">
  <?php if ($miicard):?>
  <h3><?php print "{$miicard->getFirstName()} {$miicard->getLastName()}"; ?></h3>
  <?php if ($miicard->getCardImageUrl() && $miicard->getHasPublicProfile()): ?>
  	<img class="miicard-image" alt="<?php print t('miiCard image');?>" src="<?php print $miicard->getCardImageUrl(); ?>" />  
  <?php endif; ?>
	<?php if ($miicard->getIdentityAssured()): ?>
  	<p class="assured"><?php print t("Identity assured"); ?></p>
  <?php else: ?>
    <p class="not-assured"><?php print t("Identity NOT assured!"); ?></p>
  <?php endif; ?>
  
  <dl>
  	<dt><?php print t('Last verified:')?></dt>
  	<dd>
  	  <?php 
  	    if ($miicard->getLastVerified()) {
  	      print date('j F Y, g:ia', intval($miicard->getLastVerified()));
  	    }
  	    else {
  	      print t('Unknown');
  	    }
      ?>
  	</dd>
  	<?php if ($miicard->getEmailAddresses()): ?>
  	  <dt><?php print t('Email address:'); ?></dt>
  	  <dd><?php
  	    foreach ($miicard->getEmailAddresses() as $address) {
          if ($address->getIsPrimary()) {
    	    print $address->getAddress();
    	    break;
    	  }
        }?></dd>
  	<?php endif; ?>
  	<?php if ($miicard->getPhoneNumbers()): ?>
  	  <dt><?php print t('Phone number:'); ?></dt>
  	  <dd><?php
  	    foreach ($miicard->getPhoneNumbers() as $number) {
          if ($number->getIsPrimary()) {
    	    print "+{$number->getCountryCode()} {$number->getNationalNumber()}";
    	    break;
    	  }
        }?></dd>
  	<?php endif; ?>
  </dl>
	
  <?php if ($miicard->getProfileUrl()): ?>
	  <p><a href="<?php print $miicard->getProfileUrl(); ?>">
	    <?php print t('miiCard Profile'); ?>
	  </a></p>
	<?php endif; ?>
  
  <?php if ($miicard->getIdentities()): ?>
    <ul>
      <?php foreach ($miicard->getIdentities() as $identity): ?>
      	<li><a href="<?php print $identity->getProfileUrl(); ?>">
      	  <?php print $identity->getSource(); ?>
      	</a></li>
      <?php endforeach; ?>
    </ul>
  <?php endif; ?>
  <?php else: ?>
  <h3>This user hasn't attached a miiCard to their account yet. <?php print l("What is miiCard?", "http://www.miicard.com/how-it-works", array('attributes' => array('target' => '_blank'))) ?></h3>
  <?php endif; ?>

</div>
