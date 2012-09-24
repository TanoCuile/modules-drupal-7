<?php

/**
 * @file
 * Default theme implementation to present the miiCard image for the user
 *
 * Available variables:
 * - $src: Card image URL provided by miiCard
 * - $alt: Alternative text for image
 */
?>
<?php if ($src): ?>
  <div class="miicard-image">
    <img src="<?php print $src; ?>" alt="<?php print $alt; ?>" />
  </div>
<?php endif; ?>
