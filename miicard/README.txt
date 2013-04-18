modules-drupal-7
================

Drupal 7 module to enable 'sign in with miiCard' functionality and provide a basis for deeper integrations. For more information about this and other libraries and components that interoperate with miiCard, see the [miiCard Developers](http://www.miicard.com/developers) site.

###What is miiCard?
miiCard lets anybody prove their identity to the same level of traceability as using a passport, driver's licence or photo ID. We then allow external web applications to ask miiCard users to share a portion of their identity information with the application through a web-accessible API.

###What is this Drupal module for?
This Drupal module is intended to be the basis of an integration of miiCard identity assurance into a Drupal 7-powered site. Out of the box, it supports:

* A 'sign in with miiCard' option that creates site accounts automatically as required
* A tab on a user's profile page indicating whether their identity has been assured by miiCard
* An option to attach a miiCard account to an existing user account within your site through the user's profile tab
* Mapping of miiCard user profile fields to Drupal Fields
* Automated updating of miiCard identity assurance information
* Adds a role, 'miiCard Verified' that can be used for access control

###Configuration
Once installed, configure the module by going to the Configuration... People... miiCard section of the administative pages. The first two settings you'll need to supply are:

* Consumer key
* Consumer secret

These are your OAuth credentials supplied to you when you signed up for a developer account at miiCard.com. If you don't have them, you can request developer access by filling in the form at http://www.miicard.com/developers/getting-started and letting us know the URL of your site as well as what your intended use of miiCard is.

By default, the module will:

* Add a link to your login and sign-up pages inviting the user to sign in using their miiCard account
* Add a tab to each user's profile called 'miiCard', where they can attach a miiCard to their account
* Add a miiCard identity assurance image (a little green tick glyph) next to the username of anyone who has a valid miiCard attached to their account
* Add all miiCard-verified users to the 'miiCard Verified' role, which the module creates upon installation

###Customisation

#### Automatic account creation
This is turned off by default, and is controlled by the 'Allow creation of new accounts via miiCard' option in the miiCard configuration page.

When turned on, a new user signing into your sign with their miiCard will be shown your standard sign-up form, pre-populated with their miiCard username and verified email address. When they submit the form, the newly created account is linked to their miiCard account, so that next time they are signed straight in.

#### Automatic account approval
This is also turned off by default, controlled by the 'Automatically approve miiCard-created accounts' option in the miiCard configuration page. This option is available only if you have configured your site to require administrator approval for all new accounts created - it won't appear otherwise. It also has no effect unless the 'Allow creation of new accounts via miiCard' option is enabled.

If set, accounts created via miiCard will be automatically marked as approved so long as the new user's email address is one of the verified email addresses they shared through the miiCard service. If they change their sign-up email address or don't share one then the normal account approval process takes over and the new user will be unable to sign in until you approve their access.

#### Highlighting miiCard-verified identities
When set, a small green tick glyph is shown next to the user's username in the byline of any posts or articles the user makes - so long as their identity remains assured by miiCard.

This also enables the miicard_attribution function, which you can use in templates - supply it with a comment object and it will mark up the username of the commenter in the same way.

#### Affiliate code
If you have a miiCard affiliate code, you can enter it in the configuration page. This will let miiCard attribute new miiCard members to your site. If an affiliate code is set, you can also use the miicard_url function to build URLs to the miiCard site - these URLs will have your affiliate code appended automatically.

#### Automated identity refresh
A cron job can pull updated miiCard identity information on a nightly, bi-nightly or weekly schedule. For each user who has linked a miiCard to their site account an attempt to get updated information will be made.

If there's a problem pulling back the user's miiCard information, or if their identity is no longer assured then their profile is updated to reflect that. They'll still be able to log in using the miiCard (at which point updated information will again be pulled down from the miiCard service).

### Templating
The miiCard module adds a tab to user profile pages containing some of the information shared by the miiCard member in addition to an assurance about their identity. In the /miicard module folder exist two templates, miicard-card.tpl.php and miicard-image.tpl.php that can be modified to change the set of details shown or how they are formatted.

In addition, when a user attaches a miiCard to an existing account or signs into your site using their miiCard the set of personal information that they have elected to share is stored against the user as a serialised PHP object - you can interrograte this object as required in your own code. This functionality is provided by the miiCard PHP Wrapper Libraries, and a MiiUserProfile object is stored. See the documentation for more information.

###Dependencies
The module takes a dependency (included) on OAuth.php by Andy Smith, distributed under the MIT License. It also depends upon (and is also bundled with) the miiCard API PHP Wrapper Libraries, distributed under the BSD 3-Clause licence.

###Licensing
The module is dual-licenced under the [BSD 3-Clause licence](http://opensource.org/licenses/BSD-3-Clause) and [GPL v2 licences](http://opensource.org/licenses/gpl-2.0.php) to support its provision via Drupal.org. You can choose which licence you wish to govern your use of the module, provided you adhere to all of the clauses in the licence you select. You may be forced to use one or other licence depending on your use case.

###Contributing
* Use GitHub issue tracking to report bugs in the component
* Join the [miiCard.com developer forums](http://devforum.miicard.com) to keep up to date with the latest releases and planned changes