<?php
	session_start();
	//install file for online ordering takeaway websites
	//process as follows.
	//get sitename from url
	//use for suggestion of site name
	//receive user input of site name
	//
	//1. generate db name user and password
	//2. create
	//3. create tables
	//4. create emails
	//5. save details in temp table in db
	//6. test emails
	//7. echo all stuff to screen and send copy to techie
	//end here if different set up and styler - revisiting will continue from this point
	//8. get images and css files
	//9. upload and save
	//10. ask for info for app.kfc ??? if needed
	//11. alter files with new information and complete .htaccess file. save and site should be live
	//12. remove all install files and folders...die
	function create_random()
    {
        $str = '';
        $values = array('a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v' ,'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K' ,'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9');

        for ($i = 1; $i <= 16; $i+=1)
        {
            $num = rand(0, 61);
            $str .= $values[$num];
        }
        return $str;
    }

	define('SERVER_ROOT', $_SERVER['DOCUMENT_ROOT'] . '/');
	define('HOST', $_SERVER['HTTP_HOST']);
	$dom_rep = array('www.','demo.');
	define('DOMAIN', str_replace($dom_rep,'', HOST));
	define('SITE_PATH', 'http://'.HOST.'/');
	define('STEP_FILE', SERVER_ROOT . 'step.txt');
	define('INST_DIR', SERVER_ROOT . 'inst/');
	define('INST_TEMPLATES',INST_DIR . 'templates/');
	define('INCLUDES_DIR',SERVER_ROOT . 'includes/');
	
	require_once(INST_DIR . 'cPanel.php');

	$emails = array('orders','contact','admin');

	$part = explode('/',SERVER_ROOT);
	$authfile = '/' . $part[1] . '/' . $part[2] . '/etc/test_file.txt';
	define('AUTHFILE_LOCATION', $authfile);
	define('CPUSNM',$part[2]);
	$rep_array = array('.co.uk', '.com', 'www.','demo.');
	$url = str_replace($rep_array,'' ,HOST);
	function getAuthInfo() {
		if (!file_exists(AUTHFILE_LOCATION)) {
			echo 'EXTREME ERROR: Auth file not found! Please contact support!';
			die();
		}
		else
		{
			$afh = @fopen(AUTHFILE_LOCATION,'r');
			if (!$afh) {
				echo "Error opening authfile. Please contact support.";
				die();
			}
			else
			{
				while (!feof($afh))
				{
					$tmp .= fgets($afh,999);
				}
				fclose($afh);
				$json = json_decode($tmp);
				unset($tmp);
			}

			return($json);
		}
	}

	if (!isset($_GET['step'])) 
	{
		//no step set check for existence of step file to show we have reached a certain stage.
		if (file_exists(STEP_FILE))
		{
			//read file
			$fh = @fopen(STEP_FILE,"r");
			if (!$fh) {
				echo "Error opening step file. Please contact support";
				die();
			}
			else
			{
				while (!feof($fh)) {
					$step = fgets($fh,999);
				}
				fclose($fh);
			}
		}
		else
		{
			//display first screen.
			$fname = INST_TEMPLATES . 'step1.html';
			$fh = @fopen($fname,"r");
			if (!$fh) {
				echo "Error getting step template ('" . INST_TEMPLATES . "step1.html') Please contact support";
				die();
			}
			else
			{
				fclose($fh);				
				$parts = explode('-',$url);
				if (count($parts) > 1)
				{
					foreach($parts as $word)
					{
						$finstr .= $word . ' ';
					}
				}
				else
				{
					$finstr = $parts[0];
				}
				$buffer = file_get_contents(INST_TEMPLATES . 'step1.html');
				$buffer = str_replace('%SITENAME%',$finstr,$buffer);

				$errorstring = '';
				if ($_GET['error'] == true)
				{
					$errorstring = '<div class="alert alert-block alert-error"><h3>ERROR</h3><p>The following errors were found:-</p><ul>';
					if (is_array($_SESSION['error']))
					{
						foreach ($_SESSION['error'] as $key => $value) {
							$errorstring .= '<li>' . $value . '</li>';
						}
					}
					else {
						$errorstring .= '<li>' . $_SESSION['error'] . '</li>';
					}
					$errorstring .= '</ul></div>';
				}
				$buffer = str_replace('%ERROR%', $errorstring, $buffer);
				unset($_SESSION['error']);
			}
			print_r($buffer);
			die();
		}
	}
	else
	{
		$step = $_GET['step'];
	}

	switch ($step) {
		case 1:
			//given site name
			//
			//save site name app.kfc
			//create emails, dbs and users.
			if ($_POST['orderemail'] != '')
			{
				if(preg_match("/^[a-zA-Z]+\@{1}[a-zA-Z09]([\.a-zA-Z]){2,}$/", $_POST['orderemail']) !== 1)
				{
					$_SESSION['error'] = "Invalid Order Email Address";
					header('location:' . $_SERVER['PHP_SELF'] . '?error=true');
					die();
				}
			}

			if ($_POST['sitename'] == '')
			{
				$_SESSION['error'] = "Invalid Site Name";
				header('location:' . $_SERVER['PHP_SELF'] . '?error=true');
				die();
			}

			//.htaccess file update...
			$hturl = SERVER_ROOT . '.htaccess';
			$buffer = file_get_contents($hturl);
			$buffer = str_replace("DirectoryIndex index.php", "DirectoryIndex /cms.php?v1=home index.php", $buffer);
			$buffer = str_replace("%BASEURL%", HOST, $buffer);
			file_put_contents($hturl, $buffer);
			unset($hturl,$buffer);

			$information = array();
			$information['%SITENAME%'] = $_POST['sitename'];
			$information['%SITENAMEENCODE%'] = urlencode($_POST['sitename']);
			$information['%HASH1%'] = create_random();
			$information['%HASH2%'] = create_random();
			$information['%REF%'] = $url;

			$appurl = SERVER_ROOT . 'App.kfc';
			$buffer = file_get_contents($appurl);
			foreach($information as $key => $value) {
				$buffer = str_replace($key,$value,$buffer);
			}
			file_put_contents($appurl, $buffer);

			$eminf = array();

			//emails
			$ret = getAuthInfo();
			$emailinfo = array();
			$Panel = new cPanel('hostwired.com', CPUSNM, $ret->userpass, 2083, true, 'x3');
			foreach ($emails as $email) {
				$acct = $email . '@' . DOMAIN;
				$new = $Panel->openEmailAccount($acct);
				$pass = create_random();
				$eminf['emails'][$acct] = $pass;
				$new->create($pass,250);
				array_push($emailinfo, array($acct,$pass));
				unset($new);
			}
			unset($Panel);
			$Panel = new Cpanel('hostwired.com', CPUSNM, $ret->userpass, 2083, true, 'x3');
			$orderemail = $Panel->openEmailAccount('orders@' . DOMAIN);
			$orderemail->addForwarder('techie@phdn.co.uk');
			if ($_POST['orderemail'] != '')
			{
				$orderemail->addForwarder($_POST['orderemail']);
			}
			unset($orderemail,$Panel);

			$from = 'contact@' . DOMAIN;
			$to = 'orders@' . DOMAIN;
			$subject = $_POST['sitename'] . " - Forwarder Enabled";
			$headers = "From: " . $from . "\r\nReply-to: " . $from . "\r\n";
            $headers .= "MIME-Version: 1.0" . "\r\n" . "Content-type: text/plain; charset=UTF-8" . PHP_EOL;
            $message = "A forwarder has been added to the orders account to forward all mails to techie@cheaptakeawaymenus.com. This will be removed once we are sure your system is working correctly." . PHP_EOL . "Thanks," . PHP_EOL . "The Cheap Takeawaymenus Team";
            mail($to, $subject, $message, $headers);

			//database
			$Panel = new Cpanel('hostwired.com', CPUSNM, $ret->userpass, 2083, true, 'x3');
			//DATABASE CREATION DID NOT WORK!!!!!!!
			$db = $Panel->openDatabase('online');
			$db->create();

			$dbus = $Panel->openDatabaseUser('user2');
			$dbpass = create_random();
			$dbus->create($dbpass);
			unset ($db,$dbus,$Panel);

			$Panel = new Cpanel('hostwired.com', CPUSNM, $ret->userpass, 2083, true, 'x3');

			$dbname = CPUSNM . '_online';
			$dbuser = CPUSNM . '_user2';
			$db = $Panel->openDatabase($dbname);

			$db->addUser($dbuser);

			$dbfile = INCLUDES_DIR . 'Class_DatabaseInterface2.inc';
			$buffer = file_get_contents($dbfile);
			$buffer = str_replace('%DBNAME%',$dbname,$buffer);
			$buffer = str_replace('%DBUSER%', $dbuser, $buffer);
			$buffer = str_replace('%DBPASS%', $dbpass, $buffer);
			$buffer = str_replace('%UNKNOWNUSERNAME%', '%DBUSER%', $buffer);
			file_put_contents($dbfile, $buffer);

			$eminf['db'] = array('name' => $dbname, 'user' => $dbuser, 'pass' => $dbpass);

			require_once(INCLUDES_DIR . 'Class_DatabaseInterface2.inc');
			$DBA = new DatabaseInterface();
			include(INST_DIR . 'blank_db.php');
			$DBA->setup($sqlarray);
			unset($sqlarray,$DBA);

			$message = "The first setup stage has been reached at " . $_POST['sitename'] .". \nInformation is as follows:-" . PHP_EOL;
			$message .= "---EMAILS---" . PHP_EOL;
			foreach ($eminf['emails'] as $account => $password)
			{
				$message .= $account . " - " . $password . PHP_EOL;
			}
			if ($_POST['orderemail'] != '')
			{
				$message .= "All mails to orders@" . DOMAIN . " will also be forwarded to " . $_POST['orderemail'] . "." . PHP_EOL;
			}
			$message .= PHP_EOL;
			$message .= "---DATABASE---" . PHP_EOL;
			$message .= "DbName: " . $eminf['db']['name'] .PHP_EOL;
			$message .= "DbUsername: " . $eminf['db']['user'] .PHP_EOL;
			$message .= "DbUserPass: " . $eminf['db']['pass'] .PHP_EOL;
			$to = "darren@phdn.co.uk";
			$from = 'contact@' . DOMAIN;
			$subject = $_POST['sitename'] . " - Stage 1 Complete";
			$headers = "From: " . $from . "\r\nReply-to: " . $from . "\r\n";
            $headers .= "MIME-Version: 1.0" . "\r\n" . "Content-type: text/plain; charset=UTF-8" . PHP_EOL;
            mail($to, $subject, $message, $headers);
            $to = "kevin@phdnortheast.com";
            mail($to, $subject, $message, $headers);

            //create step file
            if (file_exists(STEP_FILE))
            {
            	$res = file_get_contents(STEP_FILE);
            }
            else
            {
            	$res = "2";
            }
            file_put_contents(STEP_FILE, $res);

            $fname = INST_TEMPLATES . 'step1-fin.html';
			$fh = @fopen($fname,"r");
			if (!$fh) {
				echo "Error getting step template ('" . INST_TEMPLATES . "step1-fin.html') Please contact support";
				die();
			}
			else
			{
				$buffer = file_get_contents(INST_TEMPLATES . 'step1-fin.html');
				if($_POST['orderemail'] != '')
				{
					$message = '<p>Order email has been forwarded to:- ' . $_POST['orderemail'] . '</p>';
				}
				else
				{
					$message = '';
				}
				$buffer = str_replace('%ORDERMAILMSG%', $message, $buffer);
			}
			print_r($buffer);
			break;
		case 2:
			//second stage... get css and image files from uploader //$fname = INST_TEMPLATES . 'step1.html';
			$fname = INST_TEMPLATES . 'step2.html';
			$fh = @fopen($fname,"r");
			if (!$fh) {
				echo "Error getting step template ('" . INST_TEMPLATES . "step2.html') Please contact support";
				die();
			}
			else
			{
				fclose($fh);
				$errorstring = '';
				if ($_GET['error'] == true)
				{
					$errorstring = '<div class="alert alert-block alert-error"><h3>ERROR</h3><p>The following errors were found:-</p><ul>';
					if (is_array($_SESSION['error']))
					{
						foreach ($_SESSION['error'] as $key => $value) {
							$errorstring .= '<li>' . $value . '</li>';
						}
					}
					else {
						$errorstring .= '<li>' . $_SESSION['error'] . '</li>';
					}
					$errorstring .= '</ul></div>';
				}
				//show template file showing file inputs
				$buffer = file_get_contents(INST_TEMPLATES . 'step2.html');
				$buffer = str_replace('%ERROR%', $errorstring, $buffer);
				print_r($buffer);
			}
			break;
		case 3:
			//process file upload
			$err = false;
			$_SESSION['error'] = array();
			if (!isset($_FILES)) {
				array_push($_SESSION['error'], "Please select some files to upload!");
				$err = true;
			}
			if (!isset($_FILES['storefile']))
			{
				array_push($_SESSION['error'], "Please select a store.css file to upload!");
				$err = true;
			}
			if (!isset($_FILES['bg-img']))
			{
				array_push($_SESSION['error'], "Please select an body-bg.png image file to upload!");
				$err = true;
			}
			if (!isset($_FILES['logo']))
			{
				array_push($_SESSION['error'], "Please select a logo.png image file to upload!");
				$err = true;
			}
			if (!isset($_FILES['f-bg']))
			{
				array_push($_SESSION['error'], "Please select a f-bg.png image file to upload!");
				$err = true;
			}
			if ($err === true)
			{
				header('location:' . HOST. 'install.php?error=true');
				die();
			}
			$err = false;
			if ($_FILES['storefile']['type'] != 'text/css')
			{
				$err = true;
				array_push($_SESSION['error'], "Store.css should be of the type *.css");
			}
			if ($_FILES['bg-img']['type'] != 'image/png')
			{
				$err = true;
				array_push($_SESSION['error'], "body-bg.png should be of the type *.png");
			}
			if ($_FILES['logo']['type'] != 'image/png')
			{
				$err = true;
				array_push($_SESSION['error'], "logo.png should be of the type *.png");
			}
			if ($_FILES['f-bg']['type'] != 'image/png')
			{
				$err = true;
				array_push($_SESSION['error'], "f-bg.png should be of the type *.png");
			}
			if ($err === true)
			{
				header('location:' . HOST . 'install.php?error=true');
				die();
			}

			//ok upload the things...
			$cssfilenewname = SERVER_ROOT . 'css/store.css';
			move_uploaded_file($_FILES['storefile']['tmp_name'], $cssfilenewname);
			$bgfilenewname = SERVER_ROOT . 'img/body-bg.png';
			move_uploaded_file($_FILES['bg-img']['tmp_name'], $bgfilenewname);
			$logofilenewname = SERVER_ROOT . 'img/logo.png';
			move_uploaded_file($_FILES['logo']['tmp_name'], $logofilenewname);
			$fbgfilenewname = SERVER_ROOT . 'img/f-bg.png';
			move_uploaded_file($_FILES['f-bg']['tmp_name'], $fbgfilenewname);

			$message = "The installation of styling and image files has completed for " . HOST . PHP_EOL;
			$message .= "Please check to make sure all is correct.";
			$to = "darren@phdn.co.uk";
			$from = 'contact@' . DOMAIN;
			$subject = "Stage 2 Complete";
			$headers = "From: " . $from . "\r\nReply-to: " . $from . "\r\n";
            $headers .= "MIME-Version: 1.0" . "\r\n" . "Content-type: text/plain; charset=UTF-8" . PHP_EOL;
            mail($to, $subject, $message, $headers);
            $to = "kevin@phdnortheast.com";
            mail($to, $subject, $message, $headers);

            $fname = INST_TEMPLATES . 'step2-fin.html';
			$fh = @fopen($fname,"r");
			if (!$fh) {
				echo "Error getting step template ('" . INST_TEMPLATES . "step2-fin.html') Please contact support";
				die();
			}
			else
			{
				fclose($fh);
				$buffer = file_get_contents(INST_TEMPLATES . 'step2-fin.html');
				$buffer = str_replace('%LINK%', 'http://' . HOST, $buffer);
				print_r($buffer);
			}
           	
            file_put_contents(STEP_FILE, 'XXX');

            $dh = opendir(INST_TEMPLATES);
            while (false !== ($filename = readdir($dh))) {
            	if (($filename == '.') || ($filename == '..'))
				{
					continue;
				}
            	unlink(INST_TEMPLATES . $filename);
            }
            unset($dh);

            rmdir(INST_TEMPLATES);

            $buff = file_get_contents(INST_DIR . 'cPanel.php');
            $newf = INCLUDES_DIR . 'Class_CPanel.inc';
            file_put_contents($newf, $buff);

            $dh = opendir(INST_DIR);
			while (false !== ($filename = readdir($dh))) {
				if (($filename == '.') || ($filename == '..'))
				{
					continue;
				}

				if (!is_dir($filename))
				{
					unlink(INST_DIR . $filename);
				}
			}

			rmdir(INST_DIR);
			break;
	}
die();
?>