modules-drupal-7
================

Drupal 7 module to enable 'sign in with miiCard' functionality and provide a basis for deeper integrations. For more information about this and other libraries and components that interoperate with miiCard, see the [miiCard Developers](http://www.miicard.com/developers) site.

###What is miiCard?
miiCard lets anybody prove their identity to the same level of traceability as using a passport, driver's licence or photo ID. We then allow external web applications to ask miiCard users to share a portion of their identity information with the application through a web-accessible API.

You can find a .zip of the module ready for installation into your Drupal site on the [Downloads](https://github.com/miiCard/modules-drupal-7/downloads) tab.

###What is this Drupal module for?
This Drupal module is intended to be the basis of an integration of miiCard identity assurance into a Drupal 7-powered site. Out of the box, it supports:

* A 'sign in with miiCard' option that creates site accounts automatically as required
* A tab on a user's profile page indicating whether their identity has been assured by miiCard
* An option to attach a miiCard account to an existing user account within your site through the user's profile tab
* Mapping of miiCard user profile fields to Drupal Fields

###Configuration
Once installed, configure the module by going to the Modules... miiCard section of your admin pages. Most of the settings need not change - the two important ones are:

* Consumer key
* Consumer secret

You can request a consumer key and consumer secret from miiCard by submitting a support request including a description of the information that you would like miiCard members to share with your site. See the [Getting started](http://www.miicard.com/developers/getting-started) section of the miiCard developers site for more information.

###Customisation
The miiCard module adds a tab to user profile pages containing some of the information shared by the miiCard member in addition to an assurance about their identity. In the /miicard module folder exist two templates, miicard-card.tpl.php and miicard-image.tpl.php that can be modified to change the set of details shown or how they are formatted.

In addition, when a user attaches a miiCard to an existing account or signs into your site using their miiCard the set of personal information that they have elected to share is stored against the user as a JSON object - you can interrograte this object as required in your own code. Again, see the miicard-card.tpl.php file for example, and the [miiCard Claims API reference](http://www.miicard.com/developers/claims-api) for details on the expected structure of the JSON.

###Dependencies
The module takes a dependency (included) on OAuth.php by Andy Smith, distributed under the MIT License.

###Licensing
The module is distributed under the [BSD 3-Clause licence](http://opensource.org/licenses/BSD-3-Clause), see miicard/LICENCE.txt.

###Contributing
* Use GitHub issue tracking to report bugs in the component
* If you're going to submit a patch, please base it off the development branch
* Join the [miiCard.com developer forums](http://devforum.miicard.com) to keep up to date with the latest releases and planned changes