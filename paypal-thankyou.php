<?php
	session_start();

    $newUser = checklogged();
    unset($canonical);
    $Store = new Store();
    $Store->getStore(1);
    $location_id=0;
    $Store->setStoreVar('SetLocationID',0);
    
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

    if (isset($Page->PNumber))
    {
    	$Basket->setBasketVar('ID', $Page->PNumber);
    	$Basket->getBasket('login');
    	if ($Basket->PPReturned == 1)
		{
  			$Basket->clearBasket();
		}
		else
		{
  			$Basket->updateStatus('cusreturned');
		}
		$retarray = $Basket->getTotals();
    	$basketstr = '<span>Items = ' . $retarray['totitems'] . ' Total = &pound;' . number_format($retarray['totalcost'],2) . '</span><a data-loading-text="...loading..." data-toggle="modal" data-target="#viewModal" data-url="../view-basket/" class="btn" title="basket">Basket</a><div class="clearfix"></div>';
    }
    else
    {
    	header('location:' . SITE_PATH);
    	die();
    }

    /*if(isset($Basket2))
    {
        if ($Store->Locations[$Store->SetLocationID]['delivers'] == "Y")
        {
            $storell = array('lat' => $Store->Locations[$Store->SetLocationID]['geolat'], 'long' => $Store->Locations[$Store->SetLocationID]['geolong']);
            if (isset($newUser))
            {
                $newUser->calculateDistanceToStore($storell);
                $distance = $newUser->DistanceToStore;
            }
            else
            {
                $distance = MAX_DELIVERY_RADIUS;
            }
            $Basket2->calculateDeliveryCharge($distance);
       	}
    }*/
?>
<!DOCTYPE html>
<html lang="en">
	<head>
		<title>Bootstrap Testing</title>
		<meta charset="utf8" />
		<meta name="viewport" content="width=device-width, initial-scale=1.0">
		<meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1">
		<link rel="shortcut icon" href="/favicon.ico">
		<link rel="apple-touch-icon" href="/apple-touch-icon.png">
		<link href="../../css/bootstrap.min.css" rel="stylesheet" media="screen">
		<link href='http://fonts.googleapis.com/css?family=Archivo+Narrow:400,700|Oxygen:400,700' rel='stylesheet' type='text/css'>
		<link href="../../css/styles.css" rel="stylesheet">
		<link href="../../css/store.css" rel="stylesheet">
	</head>
	<body data-spy="scroll" data-target="#sidebarnav" itemscope itemtype="http:/schema.org/Restaurant" class="pp">
		<meta itemprop="menu" content="<?php print_r(SITE_PATH); ?>" />
		<div id="basket">
			<div class="well well-small">
				<?php print_r($basketstr); ?>
			</div>
		</div>
		<div class="container-fluid">
			<header id="header" class="row-fluid">
				<div class="span4" id="logo">
					<a href="#"><span itemprop="name"><?php print_r($Store->Name); ?></span></a>
				</div>
				<div id="tw-info" class="span4">
					<?php
						print_r($Store->Locations[$location_id]['schema_address']);
					?>
					<div class="controls controls-row">
						<a href="#delModal" class="btn span6" role="button" data-toggle="modal" data-target="#delModal" data-loading-text="Please Wait" title="Delivery Information">Delivery Information</a>
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
				<div id="items" class="span12">
					<h1>Thank you for your payment!</h1>
					<div id="holder">
						<div class="alert alert-success">Your payment was received successfully.</div>
						<p>You will receive an email at the email address you provided confirming this.</p>
						<p><?php echo SITE_NAME; ?> are now preparing your order. Please leave 45 minutes to an hour (delivery/collectionmessage here).</p>
						<p>We will be in touch if there is a problem with your order however if you have any enquiries please contact us using the telephone number above or using our <a href="#contModal" class="btn btn-link" role="button" data-toggle="modal" data-loading-text="Please Wait..." data-target="#contModal" title="Contact Us">online contact service</a>.</p>
						<a class="btn" href="<?php echo SITE_PATH; ?>" title="Return to the homepage">Home</a>
					</div>
				</div>
			</section>
		</div>
		<div class="container-fluid">
			<div id="footer" class="row-fluid">
				<div>
					<ul class="nav nav-pills">
						<li><a href="#delModal" role="button" data-toggle="modal" data-loading-text="Please Wait..." data-target="#delModal" title="Delivery Information">Delivery Information</a></li>
						<li><a href="#openModal" role="button" data-toggle="modal" data-loading-text="Please Wait..." data-target="#openModal" title="Opening Hours">Opening Hours</a></li>
						<li><a href="#contModal" role="button" data-toggle="modal" data-loading-text="Please Wait..." data-target="#contModal" title="Contact Us">Contact Us</a></li>
						<li><a href="#tandcmodal" role="button" data-toggle="modal" data-loading-text="Please Wait..." data-target="#tandcModal" data-remote="../terms-and-conditions/pop/" title="Terms & Conditions">Terms &amp; Conditions</a></li>
						<li><a href="#privacymodal" role="button" data-toggle="modal" data-loading-text="Please Wait..." data-target="#privacyModal" data-remote="../privacy-policy/pop/" title="Privacy Policy">Privacy Policy</a></li>
					</ul>
				</div>
			</div>
			<div class="row-fluid">
				<div class="span4 offset4">&copy;<?php print_r($Store->Name); ?> Ltd All Rights Reserved</div>
			</div>
			<div class="clearfix"></div>
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
			    	<div class="control-group">
			    		<label for="name" class="control-label">Name</label>
			    		<div class="controls">
			    			<input type="text" name="cont-name" placeholder="Enter a name for yourself" />
			    		</div>
			    	</div>	
			    	<div class="control-group">
			    		<label for="reg-email" class="control-label">Email<small>*</small></label>
			    		<div class="controls">
			    			<input type="email" name="cont-email" placeholder="Enter your email(Required)" required />
			    		</div>
			    	</div>
			    	<div class="control-group">
			    		<label for="reg" class="control-label">Your message<small>*</small></label>
			    		<div class="controls">
			    			<textarea name="cont-mess" placeholder="Enter a Message(Required)" required></textarea>
			    		</div>
				  	</div>
				</div>
			  	<div class="modal-footer">
			    	<button class="btn"  data-loading-text="..." data-dismiss="modal" aria-hidden="true">Cancel</button>
			    	<button class="btn btn-primary" data-loading-text="sending">Send</button>
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
		<script src="../../js/bootstrap.min.js"></script>
		<script src="../../js/functions.js"></script>
		<!--<script src="../../js/bdetect.js"></script>-->
		<script>
			$(document).ready(function() {
				preparePage();
			});
		</script>
	</body>
</html>