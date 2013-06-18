<?php 
// STEP 1: Read POST data
require_once('App.kfc');
require_once(SERVER_ROOT . 'includes/functions.inc');
require_once(SERVER_ROOT . 'includes/Class_DatabaseInterface2.inc');
require_once(SERVER_ROOT . 'includes/Class_Basket.inc');
require_once(SERVER_ROOT . 'includes/Class_User.inc');
require_once(SERVER_ROOT . 'includes/Class_Store.inc');

//$testserver = true;
$bid = explode('=',$_SERVER['argv'][0]);
$basket_id = $bid[1];
// reading posted data from directly from $_POST causes serialization 
// issues with array data in POST
// reading raw POST data from input stream instead. 
$file = $_SERVER['DOCUMENT_ROOT'] . '/test-log-pp.txt';
$fh = fopen($file, 'a');
$data = "basket_id: " . $basket_id . "\n";
fwrite($fh,$data);
fclose($fh);

$raw_post_data = file_get_contents('php://input');
$raw_post_array = explode('&', $raw_post_data);
$file = $_SERVER['DOCUMENT_ROOT'] . '/test-log-pp.txt';
$fh = fopen($file, 'a');
$data = "rpd = " . $raw_post_data. "\n";
fwrite($fh,$data);
fclose($fh);
$myPost = array();
foreach ($raw_post_array as $keyval) {
  $keyval = explode ('=', $keyval);
  if (count($keyval) == 2)
     $myPost[$keyval[0]] = urldecode($keyval[1]);
}

// read the post from PayPal system and add 'cmd'
$req = 'cmd=_notify-validate';
if(function_exists('get_magic_quotes_gpc')) {
   $get_magic_quotes_exists = true;
} 
foreach ($myPost as $key => $value) {        
   if($get_magic_quotes_exists == true && get_magic_quotes_gpc() == 1) { 
        $value = urlencode(stripslashes($value)); 
   } else {
        $value = urlencode($value);
   }
   $req .= "&$key=$value";
}
 
$fh = fopen($file, 'a');
$data = "Verifying with Paypal " . date("D jS M Y G:i:s") . "\n";
fwrite($fh,$data);
fclose($fh);
// STEP 2: Post IPN data back to paypal to validate
 if ($testserver === true)
 {
  $churl = 'https://www.sandbox.paypal.com/cgi-bin/webscr';
 }
 else
 {
  $churl = 'https://www.paypal.com/cgi-bin/webscr';
 }
$ch = curl_init($churl);
curl_setopt($ch, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
curl_setopt($ch, CURLOPT_POST, 1);
curl_setopt($ch, CURLOPT_RETURNTRANSFER,1);
curl_setopt($ch, CURLOPT_POSTFIELDS, $req);
curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, 1);
curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 2);
curl_setopt($ch, CURLOPT_FORBID_REUSE, 1);
curl_setopt($ch, CURLOPT_HTTPHEADER, array('Connection: Close'));
if( !($res = curl_exec($ch)) ) {
    curl_close($ch);
    exit;
}
curl_close($ch);
 
$fh = fopen($file, 'a');
$data = "Response from Paypal " . date("D jS M Y G:i:s") . "\n";
fwrite($fh,$data);
fclose($fh); 
// STEP 3: Inspect IPN validation result and act accordingly
 
$txn_id = $_POST['txn_id'];

$Basket = new Basket();
$Basket->setBasketVar('ID', $basket_id);
$Basket->getBasket('login');
if (strcmp ($res, "VERIFIED") == 0) {
  $fh = fopen($file, 'a');
  $data = "Verified by Paypal " . date("D jS M Y G:i:s") . "\n";
  fwrite($fh,$data);
  fclose($fh);
    // check whether the payment_status is Completed
    // check that txn_id has not been previously processed
    // check that receiver_email is your Primary PayPal email
    // check that payment_amount/payment_currency are correct
    // process payment
 
    // assign posted variables to local variables
    /*$item_name = $_POST['item_name'];
    $item_number = $_POST['item_number'];
    $payment_status = $_POST['payment_status'];
    $payment_amount = $_POST['mc_gross'];
    $payment_currency = $_POST['mc_currency'];
    $receiver_email = $_POST['receiver_email'];
    $payer_email = $_POST['payer_email'];*/


    $ret = $Basket->processAsOrder('paypal');
    $fh = fopen($file, 'a');
    $data = "Order Saved (" . $ret . ") - " . date("D jS M Y G:i:s") . "\n";
    fwrite($fh,$data);
    fclose($fh);

} else if (strcmp ($res, "INVALID") == 0) {
    // log for manual investigation
  $fh = fopen($file, 'a');
  $data = "Verification Failed " . date("D jS M Y G:i:s") . "\n";
  fwrite($fh,$data);
  fclose($fh);

  $headers = "From: info@spydawebdesign.com";
  mail('techie@phdn.co.uk','Invalid Paypal Attempt', 'There was an invalid attempt made to pay via paypal at ' . SITE_NAME . ' txnid = ' . $_POST['txn_id'], $headers);
}

if ($Basket->CustReturned == 1)
{
  $Basket->clearBasket();
}
else
{
  $Basket->updateStatus('ppreturned');
}

?>