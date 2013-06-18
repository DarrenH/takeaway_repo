<?php
	session_start();

    $newUser = checklogged();
    unset($canonical);
    $Store = new Store();
    if (isset($_SESSION['store_id']))
    {
    	$Store->getStore($_SESSION['store_id']);
    	$location_id = $_SESSION['location_id'];
    }
    else
    {
    $Store->getStore(1);
    $location_id=0;
    }
    $Store->setStoreVar('SetLocationID',$location_id);
    
    $Store->getOLMenu();
    if ($Store->Locations[$location_id]['delivers'] == 'Y')
    {
        $Store->getDeliveryDetails2();
        $Store->prepareDeliveryDetails2();
    }

    if (count($Store->OpeningHours) >= 1)
    {
        $Store->prepareOpeningHours();
    }

    if (!isset($_SESSION['store_id']))
    {
    	$_SESSION['store_id'] = $Store->ID;
    	$_SESSION['location_id'] = $Store->SetLocationID;
    	$_SESSION['menu_id'] = $Store->MenuID;
    }

    $Basket = new Basket();

	if (isset($_SESSION['basket_id']))
	{
		$Basket->setBasketVar('ID', $_SESSION['basket_id']);
		$Basket->checkExistingBasket();
	}    

    if ((isset($_SESSION['basket_id'])) && ($_SESSION['basket_id'] != ''))
    {
    	$Basket->setBasketVar('ID',$_SESSION['basket_id']);
    	$Basket->getBasket('login');
    	if (($Basket->CustReturned == 1) && ($Basket->PPReturned == 1))
    	{
    		$Basket->clearBasket();
    		unset($Basket);
    		$Basket = new Basket();
    		$basketstr = '<span>Items = 0 Total = &pound;0.00</span><a data-loading-text="...loading..." data-toggle="modal" data-target="#viewModal" data-url="../view-basket/" class="btn" title="basket">Basket</a><div class="clearfix"></div>';
    	}
    	else
    	{
	    	$retarray = $Basket->getTotals();
    		$basketstr = '<span>Items = ' . $retarray['totitems'] . ' Total = &pound;' . number_format($retarray['totalcost'],2) . '</span><a data-loading-text="...loading..." data-toggle="modal" data-target="#viewModal" data-url="../view-basket/" class="btn" title="basket">Basket</a><div class="clearfix"></div>';	
    	}
    }
    else
    {
    	$basketstr = '<span>Items = 0 Total = &pound;0.00</span><a data-loading-text="...loading..." data-toggle="modal" data-target="#viewModal" data-url="../view-basket/" class="btn" title="basket">Basket</a><div class="clearfix"></div>';
    }
?>
<!DOCTYPE html>
<html lang="en">
	<head>
		<title><?php echo SITE_NAME; ?> | Online Ordering</title>
		<meta charset="utf8" />
		<meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1">
		<link rel="shortcut icon" href="/favicon.ico">
		<link rel="apple-touch-icon" href="/apple-touch-icon.png">
		<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1">
		<link href="../css/bootstrap.min.css" rel="stylesheet" media="screen">
		<link href='http://fonts.googleapis.com/css?family=Archivo+Narrow:400,700|Oxygen:400,700' rel='stylesheet' type='text/css'>
		<link href="../css/styles.css" rel="stylesheet">
		<link href="../css/store.css" rel="stylesheet">
	</head>
	<body data-spy="scroll" data-target="#sidebarnav" itemscope itemtype="http:/schema.org/Restaurant" class="ind">
		<meta itemprop="menu" content="<?php print_r(SITE_PATH); ?>" />
		<div id="basket">
			<div class="well well-small">
				<?php print_r($basketstr); ?>
			</div>
		</div>
		<div id="bg" class="container-fluid">
			<a name="top"></a>
			<header id="header" class="row-fluid">
				<div class="span5" id="logo">
					<a href="../"><span itemprop="name"><?php print_r($Store->Name); ?></span></a>
				</div>
				<div id="tw-info" class="span3">
					<?php
						print_r($Store->Locations[$location_id]['schema_address']);
					?>
					<div class="controls controls-row">
						<a href="#delModal" class="btn span6" role="button" data-toggle="modal" data-target="#delModal" data-loading-text="Please Wait" title="Delivery Information">Delivery</a>
    	        		<a href="#openModal" class="btn span6" role="button" data-toggle="modal" data-target="#openModal" data-loading-text="Please Wait" title="Opening Hours">Opening Hours</a>
    	        	</div>
				</div>
				<div id="login" class="span4">
					<!--<form class="form-inline" action="../login/">
						<div class="controls controls-row">
							<input type="text" class="input-small" placeholder="Email" name="regmail" id="regmail" />
							<input type="text" class="input-small" placeholder="Password" name="regpass" id="regpass" />
							<label class="checkbox"><input type="checkbox" name="remember" id="remember" /><span class="cblblsm">Remember Me</span></label>
						</div>
						<div class="controls controls-row">
				  			<a href="#myModal" class="btn span6" id="register" role="button" data-loading-text="Please Wait" data-toggle="modal" data-target="#regModal">Register</a>
				  			<button type="submit" id="login" class="btn btn-primary span6" data-loading-text="logging in" title="Login">Login</button>
				  		</div>
					</form>-->
				</div>
			</header>
			<section id="main" class="row-fluid">
				<div id="catnav" class="span3">
					<div id="sidebarnav" data-spy="flap" data-offset-top="">
						<h2>Categories<span id="showmenu" class="visible-phone"></span></h2>
						<ul id="catnavul" class="nav nav-list leftnavbar">
							<?php
								echo $Store->getMenuCategoryNavigation();
							?>
						</ul>
						<div id="catulbottom"></div>
					</div>
				</div>
				<div id="items" class="span9">
					<h1>Dishes</h1>
					<div id="holder">
						<?php echo $Store->MenuItemList; ?>
					</div>
				</div>
			</section>
		</div>
		<div class="footer-background">
		<div class="container-fluid">
			<div id="footer" class="row-fluid">
					<div class="span11 offset1">
					<ul class="nav nav-pills">
						<li><a href="#delModal" role="button" data-toggle="modal" data-loading-text="Please Wait..." data-target="#delModal" title="Delivery Information">Delivery Information</a></li>
						<li><a href="#openModal" role="button" data-toggle="modal" data-loading-text="Please Wait..." data-target="#openModal" title="Opening Hours">Opening Hours</a></li>
						<li><a href="#contModal" role="button" data-toggle="modal" data-loading-text="Please Wait..." data-target="#contModal" title="Contact Us">Contact Us</a></li>
						<li><a href="#tandcModal" role="button" data-toggle="modal" data-loading-text="Please Wait..." data-target="#tandcModal" data-remote="../terms-and-conditions/pop/" title="Terms & Conditions">Terms &amp; Conditions</a></li>
						<li><a href="#privacyModal" role="button" data-toggle="modal" data-loading-text="Please Wait..." data-target="#privacyModal" data-remote="../privacy-policy/pop/" title="Privacy Policy">Privacy Policy</a></li>
						<li><a href="#cookiesModal" role="button" data-toggle="modal" data-loading-text="Please Wait..." data-target="#cookiesModal" data-remote="../cookie-policy/pop/" title="Cookie Policy">Cookie Policy</a></li>
					</ul>
				</div>
			</div>
			<div class="row-fluid">
					<div class="span4 offset4">&copy;<?php print_r(date("Y")); ?> <?php print_r($Store->Name); ?> All Rights Reserved</div>
			</div>
			<div class="row-fluid">
				<div class="span3 offset5">
					<a href="http://www.cheaptakeawaymenus.com/cheap-takeaway-websites/" title="Free websites at Cheap Takeaway Menus.com">
						<img src="<?php echo SITE_PATH; ?>img/ctmlogo.png" alt="Powered by Cheap Takeaway menus" />
					</a>
				</div>
			</div>
			<div class="clearfix"></div>
		</div>
		</div>
		<div id="contModal" class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" area-hidden="true">
			<form class="form-horizontal">
				<div class="modal-header">
					<button type="button" class="close" data-dismiss="modal" aria-hidden="true">Close &times;</button>
				    <h3 id="myModalLabel">Contact Us</h3>
				    <p>Fill in the form below and click send</p>
				    <p><small>Fields marked with '*' are required</small><p>
				</div>
				<div class="modal-body">
					<input type="hidden" name="email" id="email" placeholder="Enter an email address" />
			    	<div class="control-group">
			    		<label for="name" class="control-label">Name</label>
			    		<div class="controls">
			    			<input type="text" name="cont-name" id="cont-name" placeholder="Enter a name for yourself" />
			    		</div>
			    	</div>	
			    	<div class="control-group">
			    		<label for="reg-email" class="control-label">Email<small>*</small></label>
			    		<div class="controls">
			    			<input type="email" name="cont-email" id="cont-email" placeholder="Enter your email(Required)" required />
			    		</div>
			    	</div>
			    	<div class="control-group">
			    		<label for="reg" class="control-label">Your message<small>*</small></label>
			    		<div class="controls">
			    			<textarea name="cont-mess" id="cont-mess" placeholder="Enter a Message(Required)" required></textarea>
			    		</div>
				  	</div>
				</div>
			  	<div class="modal-footer">
			    	<button class="btn"  data-loading-text="..." data-dismiss="modal" aria-hidden="true">Cancel</button>
			    	<button class="btn btn-primary" id="send" data-loading-text="sending">Send</button>
			  	</div>
			</form>
		</div>
		<div id="regModal" class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="regModalLabel" aria-hidden="true">
			  <div class="modal-header">
			    	<button type="button" class="close" data-dismiss="modal" aria-hidden="true">Close &times;</button>
			    	<h3 id="regModalLabel">Register for an account</h3>
			    	<p>Fill in the form below and click register</p>
			    	<p><small>Fields marked with '*' are required</small><p>
			  </div>
			  <form class="form-horizontal">
				  <div class="modal-body">
				    	<div class="control-group">
				    		<label for="name" class="control-label">Name<small>*</small></label>
				    		<div class="controls">
				    			<input type="text" name="uname" id="uname" placeholder="Enter a name for yourself" required/>
				    		</div>
				    	</div>	
				    	<div class="control-group">
				    		<label for="reg-email" class="control-label">Email<small>*</small></label>
				    		<div class="controls">
				    			<input type="email" name="reg-email" id="rmail" placeholder="Enter your email(Required)" required />
				    		</div>
				    	</div>
				    	<div class="control-group">
				    		<label for="reg-pass1" class="control-label">Password<small>*</small></label>
				    		<div class="controls">
				    			<input type="text" name="reg-pass1" id="rpass1" placeholder="Enter a Password(Required)" required />
				    		</div>
				    	</div>
				    	<div class="control-group">
				    		<label for="reg-pass2" class="control-label">Password again<small>*</small></label>
				    		<div class="controls">
				    			<input type="text" name="reg-pass2" id="rpass2" placeholder="Confirm your password(Required)" required />
				    		</div>
				    	</div>
				  </div>
				  <div class="modal-footer">
				    	<button class="btn" data-dismiss="modal" data-loading-text="cancelling" aria-hidden="true">Cancel</button>
				    	<button class="btn btn-primary" id="regsub" data-loading-text="saving">Save changes</button>
				  </div>
			  </form>
		</div>
		<div id="delModal" class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="delModalLabel" aria-hidden="true">
			<div class="modal-header">
			   	<button type="button" class="close" data-dismiss="modal" aria-hidden="true">Close &times;</button>
			   	<h4 id="delModalLabel">Delivery Details</h4>
			</div>
			<div class="modal-body">
				<div id="delivery">
			   		<?php print_r($Store->DeliveryHTML); ?>
			   	</div>
			   	<div id="gmap">
			  	</div>
			</div>
			<div class="modal-footer">
			   	<button class="close" data-dismiss="modal" aria-hidden="true">Close &times;</button>
			</div>
		</div>
		<div id="openModal" class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="openModalLabel" aria-hidden="true">
			<div class="modal-header">
			   	<button type="button" class="close" data-dismiss="modal" aria-hidden="true">Close &times;</button>
			   	<h4 id="openModalLabel">Opening Hours</h4>
			   	<?php
			   		if ($Store->OHStatus == 'open')
			   		{
			   			$class=' class="success"';
			   		}
			   	?>
			   	<p<?php echo $class; ?>>Currently we are:- <?php echo $Store->OHStatus; ?></p>
			</div>
			<div class="modal-body">
			   	<p>Our opening hours are listed below:-</p>
			   	<div class="opening-hours">
					<?php print_r($Store->OHCopy);?>
			   	</div>
			</div>
			<div class="modal-footer">
			   	<button class="close" data-dismiss="modal" aria-hidden="true">Close &times;</button>
			</div>
		</div>
		<div id="tandcModal" class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="tandcModalLabel" aria-hidden="true">
			<div class="modal-header">
			   	<button type="button" class="close" data-dismiss="modal" aria-hidden="true">Close &times;</button>
			   	<h4 id="tandcModalLabel">Terms &amp; Conditions</h4>
			</div>
			<div class="modal-body">
			   	<!-- pop template code here -->
			</div>
			<div class="modal-footer">
			   	<button class="close" data-dismiss="modal" aria-hidden="true">Close &times;</button>
			</div>
		</div>
		<div id="privacyModal" class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="privacyModalLabel" aria-hidden="true">
			<div class="modal-header">
			   	<button type="button" class="close" data-dismiss="modal" aria-hidden="true">Close &times;</button>
			   	<h4 id="privacyModalLabel">Privacy Policy</h4>
			</div>
			<div class="modal-body">
			   	<!-- pop template code here -->
			</div>
			<div class="modal-footer">
			   	<button class="close" data-dismiss="modal" aria-hidden="true">Close &times;</button>
			</div>
		</div>
		<div id="cookiesModal" class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="cookiesModalLabel" aria-hidden="true">
			<div class="modal-header">
			   	<button type="button" class="close" data-dismiss="modal" aria-hidden="true">Close &times;</button>
			   	<h4 id="cookiesModalLabel">Cookies Policy</h4>
			</div>
			<div class="modal-body">
			   	<!-- pop template code here -->
			</div>
			<div class="modal-footer">
			   	<button class="close" data-dismiss="modal" aria-hidden="true">Close &times;</button>
			</div>
		</div>
		<div id="choicesextrasModal" class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="choicesextrasModalLabel" aria-hidden="true">
			
		</div>
		<div id="viewModal" class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="viewBasketLabel" aria-hidden="true">
			
		</div>
		<div id="messages" class="alert" aria-hidden="true">
			<button type="button" class="close" data-dismiss="alert">&times;</button>
			<span></span>
		</div>
		<!--<script src="http://code.jquery.com/jquery.min.js"></script>-->
		<!--[if lt IE 9]>
		    <script src="http://code.jquery.com/jquery-1.9.1.js"></script>
		<![endif]-->
		<!--[if gte IE 9]><!-->
		    <script src="http://code.jquery.com/jquery-2.0.0b2.js"></script>
		<!--<![endif]-->
		<script defer src="../../js/bootstrap.min.js"></script>
		<script defer src="../../js/functions.js"></script>
		<script>$(document).ready(function() {preparePage();});</script>
	</body>
</html>