CREATE TABLE IF NOT EXISTS `basket` (
  `id` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `user_id` bigint(20) unsigned NOT NULL,
  `store_id` bigint(20) unsigned NOT NULL,
  `location_id` bigint(20) unsigned NOT NULL,
  `menu_id` bigint(20) unsigned NOT NULL,
  `time` bigint(20) unsigned NOT NULL,
  `ppreturned` int(11) DEFAULT NULL,
  `cusreturned` int(11) DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;


CREATE TABLE IF NOT EXISTS `basket_info` (
  `id` bigint(20) NOT NULL,
  `basket_id` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `firstname` varchar(50) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `surname` varchar(100) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `email` varchar(199) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `telno` varchar(50) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `deltype` varchar(1) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `house` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `street` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `town` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `county` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `pcode` varchar(10) CHARACTER SET latin1 COLLATE latin1_general_ci DEFAULT NULL,
  `delamount` decimal(4,2) NOT NULL,
  `account` int(11) NOT NULL,
  `password` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS `basket_items` (
  `id` bigint(20) unsigned NOT NULL,
  `basket_id` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `item_id` bigint(20) unsigned NOT NULL,
  `multiprice_description` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `number_of_items` bigint(20) NOT NULL,
  `cost_single` decimal(4,2) NOT NULL,
  `hasextras` varchar(1) CHARACTER SET latin1 COLLATE latin1_general_ci DEFAULT NULL,
  `haschoices` varchar(1) CHARACTER SET latin1 COLLATE latin1_general_ci DEFAULT NULL,
  KEY `id` (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;


CREATE TABLE IF NOT EXISTS `basket_items_choices` (
  `id` bigint(20) unsigned NOT NULL,
  `basket_id` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `basket_table_id` bigint(20) unsigned NOT NULL,
  `item_id` bigint(20) unsigned NOT NULL,
  `mpdescription` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `choice_id` bigint(20) unsigned NOT NULL,
  `cost` decimal(4,2) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;


CREATE TABLE IF NOT EXISTS `basket_items_extra` (
  `id` bigint(20) unsigned NOT NULL,
  `basket_id` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `basket_table_id` bigint(20) unsigned NOT NULL,
  `item_id` bigint(20) unsigned NOT NULL,
  `extra_id` bigint(20) unsigned NOT NULL,
  `cost` decimal(4,2) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;


CREATE TABLE IF NOT EXISTS `content` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `pageid` bigint(20) unsigned NOT NULL,
  `section_id` bigint(20) unsigned DEFAULT '0',
  `item_id` bigint(20) unsigned DEFAULT '0',
  `inputname` varchar(150) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `inputvalue` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=4 ;

INSERT INTO `content` (`id`, `pageid`, `section_id`, `item_id`, `inputname`, `inputvalue`) VALUES
(1, 2, 0, 0, 'tandc-content', '<ol>\r\n<li><h5>Our website</h5><p>Your use of this website and any service contained within constitutes acceptance of these Terms & Conditions.</p></li>\r\n<li><h5>Product Pricing and Title</h5>\r\n	<ol>\r\n		<li><p>We make every effort to ensure that the pricing displayed on our website is correct. However, if an error in the pricing of a product is found we reserve the right to either cancel your order or contact you to arrange payment of any extra sum due or refund any over-payment made by you (as applicable). The processing of an order can be cancelled or corrected by us at anytime up to the shipment of that order and any related items.</p></li>\r\n		<li><p>We reserve the right to alter all product pricing without notice.</p>\r\n		<li><p>Title in any products ordered from us does not pass to you, the purchaser until we have received and processed a valid payment, and that payment has been made into our own bank account and your order has been shipped.</p></li>\r\n	</ol>\r\n</li>\r\n<li><h5>Your Order</h5>\r\n	<ol>\r\n		<li><p>When you place an order you will automatically receive a confirmation email from us to confirm your order. Your order constitutes an offer made to us to purchase the goods specified in the order.</p>\r\n		<li><p>Your offer is only accepted by us once we have emailed you to confirm the dispatch of your order.</p></li>\r\n		<li><p>Product items not included within the dispatch email are not included in the order and contract between you and us.</p></li>\r\n		<li><p>We reserve the right to delay or refuse orders where a transaction contains incomplete details or details that cannot be verified or where fraud is suspected.</p></li>\r\n		<li><p>If we are unable to reasonably ascertain these details or resolve these issues a full refund will be made against the card used at the time of purchase. No other form of refund or credit will be offered nor will a refund be made to any third party card or account.</p></li>\r\n	</ol>\r\n</li>\r\n<li><h5>Cancellation Rights, Returns and Refunds</h5>\r\n	<ol>\r\n		<li><p>Under the Consumer Protection (Distance Selling) Regulations 2000 you have a right to cancel your purchase. However, to exercise this right you must notify us in writing, (email or letter) or in person within 7 working days from the day after you receive your goods.</p></li>\r\n		<li><p>As stated above notification of cancellation must be in writing or visit to the store, a telephone call is not a valid cancellation.</p></li>\r\n		<li><p>No right of cancellation, refund or return exists under the Consumer Protection (Distance Selling) Regulations 2000 once you have used your product, unless the product is defective and you are returning it for this reason.</p></li>\r\n		<li><p>Goods that are sealed or shrink-wrapped and this is removed can only be returned if they are defective.</p></li>\r\n	</ol>\r\n</li>\r\n<li><h5>Customer Complaints</h5><p>We endeavour to respond to all customer complaints or queries within five working days.</p></li>\r\n<li><h5>Severability</h5><p>The foregoing paragraphs, sub-paragraphs and clauses of these Terms & Conditions shall be read and construed independently of each other. Should any part of this agreement or its paragraphs, sub-paragraphs or clauses be found invalid it shall not affect the remaining paragraphs, sub-paragraphs and clauses.</p></li>\r\n<li><h5>Waiver</h5><p>Failure by this store to enforce any accrued rights under these Terms & Conditions is not to be taken as or deemed to be a waiver of those rights unless we acknowledge the waiver in writing.<p></li>\r\n<li><h5>Entire Terms & Conditions</h5><p>These Terms & Conditions set out the entire agreement and understanding between you and [Insert Organisation name]. We reserve the right to change these Terms & Conditions at any time, without giving notice to you. Your statutory rights are unaffected.</p></li>\r\n<li><h5>Jurisdiction</h5><p>These Terms & Conditions shall be interpreted, construed and enforced in accordance with English law and shall be subject to the exclusive jurisdiction of the English Courts.</p></li>\r\n</ol>\r\n<p>Last updated 15/05/2013</p>'),
(2, 3, 0, 0, 'privacy-policy-content', '<h4>Data Protection Act 1998</h4>\n<p>We comply with the principles of the Data Protection Act 1998 when dealing with all data received from visitors to the site.</p> \n<h4>Our Services</h4>\n<p>We only hold the data necessary to offer services provided on our website.</p>\n<h4>Required Period</h4>\n<p>We only hold personal data for as long as necessary. Once data is no longer needed it is deleted from our files.</p>\n<h4>Data Storage</h4>\n<p>For administrative reasons data may be passed to and stored securely with third party service providers located outside the EEA (European Economic Area).</p>\n<h4>Our Promise</h4>\n<p>We never sell, rent or exchange mailing lists.</p>\n<h4>Data Shared With Partners</h4>\n<p>We may however share commercial and technical data with our partners where a customer has accessed and used our website via a site belonging to one of our partners. However, such information will also be subject to our partners’ privacy policies.</p>\n<h4>Partner Privacy Policies</h4>\n<p>Please note that we only share data with partners that operate their own privacy policy.</p>\n<h4>Spam</h4>\n<p>In accordance with the Privacy and Electronic Communications (EC Directive) Regulations 2003, we never send bulk unsolicited emails, (popularly known as Spam) to email addresses.</p>\n<h4>Product Updates</h4>\n<p>We may send emails to existing customers or prospective customers who have enquired or registered with us, regarding products or services directly provided by us.</p>\n<h4>Cookies</h4>\n<p>Our website uses "cookies" to track use and allow customers to purchase from our website. Please note that these cookies do not contain or pass any personal, confidential or financial information or any other information that could be used to identify individual visitors or customers purchasing from our website. Please note that you are free to refuse cookies. However, for purely technical reasons this may prevent you from purchasing from our website. This is because anonymous cookies are commonly used to keep track of the contents of customers’ shopping baskets or trolleys during the checkout process. This facility ensures that the items added to (or removed from) your basket are accurately stated when you go to pay. Please see our separate Cookies Policy.</p>\n<h4>Contact Us</h4>\n<p>If you have any questions relating to our Privacy Policy please contact us using the form provided from the contact us link</p>\n<p><small>This policy was last updated on 15-05-2013</small></p>'),
(3, 5, 0, 0, 'cookie-policy-content', '<h4>What are cookies?</h4>\r\n<p>Cookies are small files containing a short string of numbers and letters placed automatically by a website into the cookie folder in your browser. Cookies are generally used to make a website easier and faster to use. The cookie recognises that a device is or has accessed the website and acts accordingly, (depending on what the cookie is designed to do). Mostly cookies perform mundane, but necessary tasks.</p>\r\n<h4>How do we use cookies?</h4>\r\n<p>We typically use cookies for the following purposes:</p>\r\n<ul>\r\n	<li>Ensure all parts of the website requested by visitors work correctly and appear quickly.</li>\r\n	<li>Alert us whenever a page on the website or the entire website is slow and not working at all.</li\r\n	<li>Analyse how visitors use our website, so that we can continually improve it by adding more information and services and making it easier to navigate and use.</li>\r\n	<li>To ensure that products you as a customer add to your shopping basket remain in your basket as you go through the checkout process, so you get what you pay for.</li>\r\n	<li>To remember you as a customer.</li>\r\n	<li>To allow customers and visitors to share our website content on social media and with friends.</li>\r\n</ul>\r\n<h4>Consent and cookies</h4>\r\n<p>Some cookies used by us are sent to your computer or mobile device as soon as the first page you view on our website is displayed. As a result we infer consent for these and other cookies if you use our website.</p>\r\n<h4>No personal information in cookies</h4>\r\n<p>None of the cookies we use contain or pass personal, confidential, financial information or any other information that could be used to identify individual visitors to the website.</p>\r\n<h4>Your shopping basket and cookies</h4>\r\n<p>If you buy a product from our website we do use cookies to pass your contact details through the checkout process. But we never request or store any financial details in this way. Your financial details are only ever passed using an encrypted and secure format from our secure payment page to your own bank for processing.</p>\r\n<h4>Refusing cookies</h4>\r\n<p>Using your browser settings you are free to refuse cookies. However, for purely technical reasons this may cause errors when trying to use our website.</p>');


CREATE TABLE IF NOT EXISTS `forgotten_passwords_link` (
  `user_id` bigint(20) unsigned NOT NULL,
  `hash` varchar(100) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `used` int(11) NOT NULL DEFAULT '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;


CREATE TABLE IF NOT EXISTS `mailqueue` (
  `id` bigint(20) NOT NULL,
  `email` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `text` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `subject` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `alias` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;


CREATE TABLE IF NOT EXISTS `main_tab_list` (
  `id` bigint(20) unsigned NOT NULL,
  `name` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

INSERT INTO `main_tab_list` (`id`, `name`) VALUES
(0, 'Starters'),
(1, 'Mains'),
(2, 'Sundries'),
(3, 'Desserts'),
(4, 'Drinks'),
(5, 'Meal Deals');


CREATE TABLE IF NOT EXISTS `menu` (
  `id` bigint(20) unsigned NOT NULL,
  `store_id` bigint(20) unsigned NOT NULL,
  `location_id` bigint(20) unsigned NOT NULL DEFAULT '0',
  `name` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `description` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `filename` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `timed` int(11) NOT NULL,
  `status` int(10) unsigned NOT NULL,
  UNIQUE KEY `id` (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;


INSERT INTO `menu` (`id`, `store_id`, `location_id`, `name`, `description`, `filename`, `timed`, `status`) VALUES
(1, 1, 0, 'Main', 'Main menu', '', 0, 1);

-- --------------------------------------------------------

--
-- Table structure for table `menu_categories`
--

CREATE TABLE IF NOT EXISTS `menu_categories` (
  `id` bigint(20) unsigned NOT NULL,
  `menu_id` bigint(20) unsigned NOT NULL,
  `name` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `cuisine_type_id` bigint(20) unsigned NOT NULL,
  `description` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `main_tab` int(10) unsigned NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;



CREATE TABLE IF NOT EXISTS `menu_item` (
  `id` bigint(20) unsigned NOT NULL,
  `menu_id` bigint(20) unsigned NOT NULL,
  `category` bigint(20) unsigned NOT NULL,
  `name` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `description` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `ptype` enum('S','M') COLLATE utf8_unicode_ci NOT NULL,
  `cost` decimal(4,2) NOT NULL,
  `vegetarian` enum('Y','N') COLLATE utf8_unicode_ci NOT NULL DEFAULT 'N',
  `vegan` enum('Y','N') COLLATE utf8_unicode_ci NOT NULL DEFAULT 'N',
  `spice_rating` enum('0','1','2','3','4','5') COLLATE utf8_unicode_ci NOT NULL DEFAULT '0',
  `mmi` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `choice_type` varchar(5) CHARACTER SET latin1 COLLATE latin1_general_ci DEFAULT NULL,
  `choice_mandatory` varchar(1) CHARACTER SET latin1 COLLATE latin1_general_ci DEFAULT NULL,
  `noofchoices` int(10) unsigned DEFAULT NULL,
  `choice_from_multiprice` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `master_menu_id` bigint(20) unsigned DEFAULT NULL,
  KEY `id` (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;


CREATE TABLE IF NOT EXISTS `menu_item_extras` (
  `id` bigint(20) unsigned NOT NULL,
  `store_id` bigint(20) unsigned NOT NULL,
  `location_id` bigint(20) unsigned NOT NULL,
  `menu_id` bigint(20) unsigned NOT NULL,
  `item_id` bigint(20) unsigned NOT NULL,
  `mpdescription` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `name` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `cost` double(4,2) NOT NULL,
  `master_extra_category` bigint(20) unsigned DEFAULT NULL,
  `master_extra_id` bigint(20) unsigned DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;


CREATE TABLE IF NOT EXISTS `menu_item_multiple_pricing` (
  `id` bigint(20) unsigned NOT NULL,
  `menu_item_id` bigint(20) unsigned NOT NULL,
  `description` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `cost` decimal(4,2) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;


CREATE TABLE IF NOT EXISTS `menu_item_xor_choices` (
  `id` bigint(20) unsigned NOT NULL,
  `store_id` bigint(20) unsigned NOT NULL,
  `location_id` bigint(20) unsigned NOT NULL,
  `menu_id` bigint(20) unsigned NOT NULL,
  `item_id` bigint(20) unsigned NOT NULL,
  `mpdescription` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `name` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `cost` double(4,2) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;


CREATE TABLE IF NOT EXISTS `opening_hours` (
  `id` bigint(20) unsigned NOT NULL,
  `store_id` bigint(20) unsigned NOT NULL,
  `location_id` bigint(20) unsigned NOT NULL,
  `mon_day` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `mon_night` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `tues_day` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `tues_night` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `weds_day` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `weds_night` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `thurs_day` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `thurs_night` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `fri_day` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `fri_night` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `sat_day` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `sat_night` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `sun_day` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `sun_night` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  UNIQUE KEY `id` (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;



CREATE TABLE IF NOT EXISTS `orders` (
  `id` bigint(20) unsigned NOT NULL,
  `user_id` bigint(20) unsigned NOT NULL,
  `store_id` bigint(20) unsigned NOT NULL,
  `menu_id` bigint(20) unsigned NOT NULL,
  `time` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `location_id` int(10) unsigned NOT NULL,
  `reference` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `discount` decimal(4,2) NOT NULL,
  `deliverycost` decimal(4,2) NOT NULL,
  `deliverytype` varchar(1) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `status` varchar(1) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL DEFAULT 'U',
  `ready_time` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `printer_message` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `processed_time` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `house` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `street` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `town` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `county` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `pcode` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `contacttelephone` bigint(20) unsigned NOT NULL,
  `contactemail` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `paymenttype` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `payment_status` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `paypaltxn` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  KEY `id` (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;


CREATE TABLE IF NOT EXISTS `order_items` (
  `id` bigint(20) unsigned NOT NULL,
  `order_id` bigint(20) unsigned NOT NULL,
  `item_id` bigint(20) unsigned NOT NULL,
  `name` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `multiprice_description` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `number_of_items` int(11) NOT NULL,
  `cost_single` decimal(4,2) NOT NULL,
  `hasextras` varchar(1) CHARACTER SET latin1 COLLATE latin1_general_ci DEFAULT NULL,
  `haschoices` varchar(1) CHARACTER SET latin1 COLLATE latin1_general_ci DEFAULT NULL,
  KEY `id` (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;


CREATE TABLE IF NOT EXISTS `order_items_choices` (
  `id` bigint(20) unsigned NOT NULL,
  `order_id` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `order_table_id` bigint(20) unsigned NOT NULL,
  `item_id` bigint(20) unsigned NOT NULL,
  `choice_id` bigint(20) unsigned NOT NULL,
  `cost` decimal(4,2) NOT NULL,
  `name` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;


CREATE TABLE IF NOT EXISTS `order_items_extra` (
  `id` bigint(20) unsigned NOT NULL,
  `order_id` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `order_table_id` bigint(20) unsigned NOT NULL,
  `item_id` bigint(20) unsigned NOT NULL,
  `extra_id` bigint(20) unsigned NOT NULL,
  `cost` decimal(4,2) NOT NULL,
  `name` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

CREATE TABLE IF NOT EXISTS `order_reference` (
  `id` int(10) unsigned NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;


INSERT INTO `order_reference` (`id`) VALUES
(100);


CREATE TABLE IF NOT EXISTS `pages` (
  `id` bigint(20) unsigned NOT NULL,
  `name` varchar(50) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `description` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `templatefile` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `poptemplatefile` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `seclevel` int(11) NOT NULL DEFAULT '2',
  `dispmm` tinyint(4) NOT NULL DEFAULT '0'
) ENGINE=MyISAM DEFAULT CHARSET=latin1;


INSERT INTO `pages` (`id`, `name`, `description`, `templatefile`, `poptemplatefile`, `seclevel`, `dispmm`) VALUES
(1, 'home', 'Home Page', 'indextest.stpl.php', '', 3, 0),
(2, 'terms-and-conditions', 'Content for the terms and conditions of the site', 'terms_and_conditions.stpl.php', 'pops/terms_and_conditions.stpl.php', 3, 0),
(3, 'privacy-policy', 'The content for the privacy statement', 'privacy.stpl.php', 'pops/privacy.stpl.php', 3, 0),
(4, 'view-basket', 'look at your basket', 'view-basket.stpl.php', 'pops/view-basket.stpl.php', 3, 0),
(5, 'cookie-policy', 'Cookie Policy Page', 'cookie_policy.stpl.php', 'pops/cookie_policy.stpl.php', 3, 0);


CREATE TABLE IF NOT EXISTS `sections` (
  `id` bigint(20) unsigned NOT NULL,
  `name` varchar(50) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `description` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `templatefile` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `seclevel` int(11) NOT NULL DEFAULT '2',
  `dispmm` tinyint(4) NOT NULL DEFAULT '0',
  `section_item` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;


CREATE TABLE IF NOT EXISTS `sentmail` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `email` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `text` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `subject` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `sentdate` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `opened` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=1 ;


CREATE TABLE IF NOT EXISTS `stores` (
  `id` bigint(20) unsigned NOT NULL,
  `owner_id` bigint(20) unsigned DEFAULT NULL,
  `name` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `urlname` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `type` enum('T','R','B') COLLATE utf8_unicode_ci NOT NULL DEFAULT 'R',
  `website` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `cuisine_type` int(11) NOT NULL,
  `halal` enum('Y','N') CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL DEFAULT 'N',
  `logo` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  KEY `id` (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;


CREATE TABLE IF NOT EXISTS `store_delivery_info` (
  `id` bigint(20) unsigned NOT NULL,
  `store_id` bigint(20) unsigned NOT NULL,
  `location_id` bigint(20) unsigned NOT NULL,
  `var1` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `var2` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `var3` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  UNIQUE KEY `id` (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;


CREATE TABLE IF NOT EXISTS `store_delivery_type` (
  `id` bigint(20) unsigned NOT NULL,
  `store_id` bigint(20) unsigned NOT NULL,
  `location_id` bigint(20) unsigned NOT NULL,
  `type` bigint(20) unsigned NOT NULL,
  `choices` varchar(3) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `max_radius` bigint(20) unsigned NOT NULL,
  `min_order_value` decimal(4,2) unsigned NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;



CREATE TABLE IF NOT EXISTS `store_locations` (
  `id` bigint(20) unsigned NOT NULL,
  `store_id` bigint(20) unsigned NOT NULL,
  `location_id` int(10) unsigned NOT NULL,
  `house` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `address_line_2` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `street` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `address_line_4` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `town` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `county` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `pcode` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `contactphone` varchar(20) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `secondaryphone` varchar(20) CHARACTER SET latin1 COLLATE latin1_general_ci DEFAULT NULL,
  `contactemail` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `orderemail` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `geolat` varchar(20) COLLATE utf8_unicode_ci DEFAULT NULL,
  `geolong` varchar(20) COLLATE utf8_unicode_ci DEFAULT NULL,
  `delivers` enum('Y','N') COLLATE utf8_unicode_ci NOT NULL,
  `delivery_type` enum('1','2','3') COLLATE utf8_unicode_ci NOT NULL,
  `imei` varchar(15) COLLATE utf8_unicode_ci DEFAULT NULL,
  `api_password` varchar(50) COLLATE utf8_unicode_ci DEFAULT NULL,
  `online` varchar(1) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `mindeliveryamount` decimal(4,2) DEFAULT NULL,
  `last_order_time` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `total_visitor_count` bigint(20) unsigned NOT NULL,
  `ppadd` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `pdq` int(10) unsigned NOT NULL DEFAULT '0',
  `pdqphone` int(10) unsigned NOT NULL DEFAULT '0',
  `pdqmobile` int(10) unsigned NOT NULL DEFAULT '0',
  `mastercard` int(11) NOT NULL,
  `maestro` int(11) NOT NULL,
  `visael` int(11) NOT NULL,
  `visa` int(11) NOT NULL,
  `solo` int(11) NOT NULL,
  `switch` int(11) NOT NULL,
  `maxradius` double NOT NULL,
  KEY `id` (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

CREATE TABLE IF NOT EXISTS `store_visitor_log` (
  `id` bigint(20) unsigned NOT NULL,
  `store_id` bigint(20) unsigned NOT NULL,
  `location_id` bigint(20) unsigned NOT NULL,
  `user_id` bigint(20) unsigned NOT NULL,
  `visitdate` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  UNIQUE KEY `store_id` (`store_id`,`location_id`,`user_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;


CREATE TABLE IF NOT EXISTS `temp_users` (
  `order_id` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `userref` bigint(20) unsigned NOT NULL,
  `time` bigint(20) unsigned NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;


CREATE TABLE IF NOT EXISTS `user` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `username` varchar(50) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `password` varchar(50) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `contactemail` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `pcode` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `optout` int(11) NOT NULL,
  `auth_level` int(11) NOT NULL,
  `logged_in` tinyint(1) NOT NULL,
  `registered_date` timestamp NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `contacttelephone` text NOT NULL,
  `mobile` text,
  `newsletter` int(11) NOT NULL,
  `admin_cp_view` int(11) NOT NULL DEFAULT '0',
  `house` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `street` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `town` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `county` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `billing_address1` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `billing_address2` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `billing_address3` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `billing_address4` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  `billing_pcode` varchar(8) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `store_owner` int(11) NOT NULL DEFAULT '0',
  `title` varchar(4) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `first_name` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `last_name` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `dob` varchar(10) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `geolat` varchar(20) DEFAULT NULL,
  `geolong` varchar(20) DEFAULT NULL,
  `avatar` text CHARACTER SET latin1 COLLATE latin1_general_ci,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=3 ;


CREATE TABLE IF NOT EXISTS `widgets` (
  `id` bigint(20) NOT NULL,
  `name` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `template_file` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `associated_section` bigint(20) NOT NULL,
  KEY `id` (`id`),
  KEY `id_2` (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;