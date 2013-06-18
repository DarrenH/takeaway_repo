<?php
    session_start();
    require_once('App.kfc');
    require_once(SERVER_ROOT . 'includes/functions.inc');
    require_once(SERVER_ROOT . 'includes/Class_DatabaseInterface2.inc');
    require_once(SERVER_ROOT . 'includes/Class_Store.inc');

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
    $contactemail = $Store->Locations[$Store->SetLocationID]['contactemail'];

    $fail = false;

    if (check_email_address($_GET['email']))
    {
        $_GET['message'] = str_replace("'", "\'", $_GET['message']);
        $_GET['message'] = strip_tags($_GET['message']);
    
        $titletwo = "Your enquiry to " . SITE_NAME;
        mail ($_GET['email'],$titletwo, "Thank you for contacting " . SITE_NAME . "\n\nWe will be in contact within 24 hours.\n\n" ,"From:" . $contactemail . "\r\nReply-to: " . $contactemail);

        $titleone = "Enquiry from " . SITE_NAME;
        $headers = "From: " . $contactemail . "\r\nReply-to: " . $_GET['email'];
      //mail to client
        mail ($contactemail, $titleone, "The following person sent a message through the ". SITE_NAME . " website contact page.\n\nName:" . $_GET['name'] . "\n\nEmail: " . $_GET['email'] . "\n\nMessage Reads:-\n\n" . $_GET['message'] . "\n\nReply to this mail to contact the person in question.\n\n" ,$headers);

        $msg = '<p><strong>Your message was sucessfully sent.</strong></p>';
		$msg .= '<p>We have sent you a copy of the message you just sent us (for your records). If you wish you could whitelist the email address in your email software that way you won\'t accidentally mark us as spam.</p>';
    }
    else
    {
        $fail = true;
    }

    //update the posttime to reflect current time
    unset($_GET);
    if ($fail == true)
    {
        $str .= '<form class="form-horizontal">';
        $str .= '<div class="modal-header">';
        $str .= '<button type="button" class="close" data-dismiss="modal" aria-hidden="true">Close &times;</button>';
        $str .= '<h3 id="myModalLabel">Contact Us</h3>';
        $str .= '<div class="alert alert-block alert-error">There was an error sending your message</div>';
        $str .='<p>Fill in the form below and click send</p>';
        $str .='<p><small>Fields marked with \'*\' are required</small><p>';
        $str .='</div>';
        $str .='<div class="modal-body">';
        $str .='<input type="hidden" name="email" id="email" placeholder="Enter an email address" />';
        $str .='<div class="control-group">';
        $str .='<label for="name" class="control-label">Name</label>';
        $str .='<div class="controls">';
        $str .='<input type="text" name="cont-name" id="cont-name" placeholder="Enter a name for yourself"value="' . $_GET['name'] . '" />';
        $str .='</div>';
        $str .='</div>  ';
        $str .='<div class="control-group error">';
        $str .='<label for="reg-email" class="control-label">Email<small>*</small></label>';
        $str .='<div class="controls">';
        $str .='<input type="email" name="cont-email" id="cont-email" placeholder="Enter your email(Required)" required />';
        $str .='<span class="help-inline">You must enter a email address.</span>';
        $str .='</div>';
        $str .='</div>';
        $str .='<div class="control-group">';
        $str .='<label for="reg" class="control-label">Your message<small>*</small></label>';
        $str .='<div class="controls">';
        $str .='<textarea name="cont-mess" id="cont-mess" placeholder="Enter a Message(Required)" required>' . $GET['message'] . '</textarea>';
        $str .='</div>';
        $str .='</div>';
        $str .='</div>';
        $str .='<div class="modal-footer">';
        $str .='<button class="btn"  data-loading-text="..." data-dismiss="modal" aria-hidden="true">Cancel</button>';
        $str .='<button class="btn btn-primary" id="send" data-loading-text="sending">Send</button>';
        $str .='</div>';
        $str .='</form>';
    }
    else
    {
        $str .= '<div class="modal-header">';
        $str .= '<button type="button" class="close" data-dismiss="modal" aria-hidden="true">Close &times;</button>';
        $str .= '<h3 id="myModalLabel">Message Sent!</h3>';
        $str .= '</div>';
        $str .= '<div class="modal-body">';
        $str .= $msg;
        $str .= '</div>';
        $str .= '<div class="modal-footer">';
        $str .= '<button class="btn"  data-loading-text="..." data-dismiss="modal" aria-hidden="true">Close</button>';
        $str .= '</div>';
}

    $retarray = array('htmlstr' => $str);

    print_r(json_encode($retarray));
    die();

?>
