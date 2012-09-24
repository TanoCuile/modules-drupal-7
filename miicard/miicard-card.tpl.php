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
  <h3><?php print "${miicard['FirstName']} ${miicard['LastName']}"; ?></h3>
  <?php if ($miicard['CardImageUrl']): ?>
  	<img class="miicard-image" alt="<?php print t('miiCard image');?>" src="<?php print $miicard['CardImageUrl']; ?>" />  
  <?php endif; ?>
	<?php if ($miicard['IdentityAssured']): ?>
  	<p class="assured"><?php print t("Identity assured"); ?></p>
  <?php else: ?>
    <p class="not-assured"><?php print t("Identity NOT assured!"); ?></p>
  <?php endif; ?>
  
  <dl>
  	<dt><?php print t('Last verified:')?></dt>
  	<dd>
  	  <?php 
  	    if ($miicard['LastVerified']) {
  	      print date('j F Y, g:ia', intval($miicard['LastVerified']));
  	    }
  	    else {
  	      print t('Unknown');
  	    }
      ?>
  	</dd>
  	<?php if (!empty($miicard['EmailAddress'])): ?>
  	  <dt><?php print t('Email address:'); ?></dt>
  	  <dd><?php print $miicard['EmailAddress']['Address']; ?></dd>
  	<?php endif; ?>
  	<?php if (!empty($miicard['PhoneNumber'])): ?>
  	  <dt><?php print t('Phone number:'); ?></dt>
  	  <dd>+<?php print $miicard['PhoneNumber']['CountryCode'] . ' ' . $miicard['PhoneNumber']['NationalNumber']; ?></dd>
  	<?php endif; ?>
  </dl>
	
  <?php if (!empty($miicard['ProfileUrl'])): ?>
	  <p><a href="<?php print $miicard['ProfileUrl']; ?>">
	    <?php print t('miiCard Profile'); ?>
	  </a></p>
	<?php endif; ?>
  
  <?php if (!empty($miicard['Identities'])): ?>
    <ul>
      <?php foreach ($miicard['Identities'] as $identity): ?>
      	<li><a href="<?php print $identity['ProfileUrl']; ?>">
      	  <?php print $identity['Source']; ?>
      	</a></li>
      <?php endforeach; ?>
    </ul>
  <?php endif; ?>    
  
</div>
