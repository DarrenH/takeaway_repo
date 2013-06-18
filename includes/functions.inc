<?php
    function loginuser($user, $pass, $verify=null, $secure=0)
    {
        $hashpass = stripslashes(encryption($pass));
        $fields = array(0 => 'password', 1 => 'id', 2 => 'store_owner');
        if ($secure == 0)
        {
            $idfields = array(0 => 'contactemail');
        }
        else
        {
            $idfields = array(0 => 'username');
        }

        $idvals = array(0 => $user);

        $DBA = new DatabaseInterface();

        $rs = $DBA->selectQuery(DBUSERTABLE, $fields, $idfields, $idvals);

        if (isset($rs[0]))
        {
            if ($rs[1] == 0)
            {
                if (!isset($verify))
                {
                    uptries();
                    checktries();
                }

                return false;
            }
            while ($res = mysql_fetch_array($rs[0]))
            {
                if ($hashpass != $res['password'])
                {
                    if (!isset($verify))
                    {
                        uptries();
                        checktries();
                    }

                    return false;
                }
                else
                {
                    //user is good lets set up object and send them on their way
                    global $User;
                    if ($res['storeowner'] == 1)
                    {
                        $User = new Supplier();
                    }
                    else
                    {
                        $User = new Customer();
                    }
                    $User->setUserVar('ID',$res['id']);
                    $User->getUser();

                    if ($secure == 1)
                    {
                        //this is a secure page check authlevel
                        if (($User->AuthLevel != "1") && ($User->AuthLevel != "0"))
                        {
                             unset($User);
                             session_push('You are not authorised to enter this part of the site.');
                             if (!isset($verify))
                             {
                                 uptries();
                                 checktries();
                             }
                             return false;
                        }
                    }

                    session_push('You were successfully logged in to the system.');

                    $_SESSION['user'] = array('name' => $User->name, 'email' => $user, 'password' => $pass, 'verified' => '1', 'verifytime' => time(), 'id' => $User->ID);

                    check_orphaned_cart($User->ID);
                    return true;
                }
            }
        }
        else
        {
            if (!isset($verify))
            {
                uptries();
                checktries();
            }

            return false;
        }
    }

    function checklogged($secure=0)
    {
        //if secure = 1 then we are in admin section and need to check everything and then auth level
        switch ($secure)
        {
            case 0:
                //non secure page just do the usual
                //check if user is verified
                if (($_SESSION['user']['verified'] != 1) || (($_SESSION['user']['verfied'] == 1) && (time() <= ($_SESSION['user']['verifytime'] + 60000))))
                {
                    unset($_SESSION['user']);
                    if (isset($RegUser))
                    {
                        unset($RegUser);
                    }
                }
                else
                {
        			global $RegUser;
                    $RegUser = new Customer();
                    $RegUser->setUserVar('ID', $_SESSION['user']['id']);
                    $RegUser->getUser();
                    return($RegUser);
                }
                break;
            case 1:
                //secure page check credentials and auth level
                //check if user is verified
                if (($_SESSION['user']['verified'] != 1) || (($_SESSION['user']['verfied'] == 1) && (time() <= ($_SESSION['user']['verifytime'] + 60000))))
                {
                    unset($_SESSION['user']);
                    if (isset($RegUser))
                    {
                        unset($RegUser);
                    }
                    header ('location: ' . LOGINPAGE);
                    die();
                }
                else
                {
        			global $regUser;
                    $RegUser = new Customer();
                    $RegUser->setUserVar('ID', $_SESSION['user']['id']);
                    $RegUser->getUser();
                    $RegUser->setUserVar('AdminCPLink', $User->getViewLink());
                    if ($RegUser->AuthLevel >= 2)
                    {
                        unset($_SESSION['user']);
                        if (isset($RegUser))
                        {
                            unset($RegUser);
                        }
                        header ('location: ' . LOGINPAGE);
                        die();
                    }
                }
                break;
        }
    }

    function uptries()
    {
        if (isset($_SESSION['tries']))
        {
            $_SESSION['tries']+=1;
        }
        else
        {
            $_SESSION['tries'] = 1;
        }
    }

    function checktries()
    {
        if ($_SESSION['tries'] >= 3)
        {
            //kick out thats too many bad entries
            if (1 == 0)
            {
                session_push('You have reached the maximum number of wrong attempts to gain entry. If you are the owner of this site please contact your support to rememdy the problem');
                setcookie('TKB','maxattempts',time()+600*600*240*300, '/thekore/');
            }
            else
            {
                header('location: ' . FORGOTTEN_PASSWORD_LINK);
                die();
            }
        }
    }

    function session_push($value)
	{
		if (isset($_SESSION['messages'][0]))
		{
			array_push($_SESSION['messages'], $value);
		}
		else
		{
			$_SESSION['messages'][0] = $value;
		}
	}

    function error_push($value)
	{
		if (isset($_SESSION['errors'][0]))
		{
			array_push($_SESSION['errors'], $value);
		}
		else
		{
			$_SESSION['errors'][0] = $value;
		}
	}

    function show_messages($page=null)
    {
        unset($str);

        if (isset($_SESSION['messages']))
        {
            $str .= '<h3>Attention</h3>';
            $str .= showmessages($page);

        }

        if (isset($_SESSION['errors']))
        {
            $str .= '<h3>Error</h3>';
            $str .= showerrors($page);
        }

        if (isset($str))
        {
            $ret = $str;
        }
        else
        {
            $ret = "false";
        }
        unset($_SESSION['messages'], $_SESSION['errors']);

        return $ret;
    }

    function showmessages($page=null)
    {
        if (isset($_SESSION['messages']))
        {
            foreach ($_SESSION['messages'] as $msg)
            {
                $msgstr .= '<li class="message">' . $msg . '</li>';
            }

            unset($_SESSION['messages']);
            return '<ul id="messages-list" class="messages' . (isset($page) ? ' ' . $page : '') . '">' . $msgstr . '</ul>';
        }
    }

    function showerrors($page=null)
    {
        if (isset($_SESSION['errors']))
        {
            foreach ($_SESSION['errors'] as $msg)
            {
                $msgstr .= '<li class="error">' . $msg . '</li>';
            }

            unset($_SESSION['errors']);
            return '<ul id="errors-list" class="errors' . (isset($page) ? ' ' . $page : '') . '">' . $msgstr . '</ul>';
        }
    }

	function check_email_address($email)
    {
        // First, we check that there's one @ symbol, and that the lengths are right
        if (!ereg("^[^@]{1,64}@[^@]{1,255}$", $email))
        {
            // Email invalid because wrong number of characters in one section, or wrong number of @ symbols.
            return false;
        }
        // Split it into sections to make life easier
        $email_array = explode("@", $email);
        $local_array = explode(".", $email_array[0]);
        for ($i = 0; $i < sizeof($local_array); $i++)
        {
            if (!ereg("^(([A-Za-z0-9!#$%&'*+/=?^_`{|}~-][A-Za-z0-9!#$%&'*+/=?^_`{|}~\.-]{0,63})|(\"[^(\\|\")]{0,62}\"))$", $local_array[$i]))
            {
                return false;
            }
        }
        if (!ereg("^\[?[0-9\.]+\]?$", $email_array[1]))
        {
            // Check if domain is IP. If not, it should be valid domain name
            $domain_array = explode(".", $email_array[1]);
            if (sizeof($domain_array) < 2)
            {
                return false; // Not enough parts to domain
            }
            /*for ($i = 0; $i < sizeof($domain_array); $i++)
            {
                //removed due to occasional clash with valid email :S
                if (!ereg("^(([A-Za-z0-9][A-Za-z0-9-]{0,61}[A-Za-z0-9])|([A-Za-z0-9]+))$", $domain_array[$i]))
                {
                    return false;
                }
            }*/
        }
        return true;
    }

	function getSiteSettings()
	{
		global $ssfound;

		$DBA = new DatabaseInterface();
		$fields = array(0 => 'site_name', 1 => 'site_status', 2 => 'subcats', 3 => 'useowneremail', 4 => 'alternateemail', 5 => 'resizeimgs', 6 => 'site_description');

		$rs = $DBA->selectQuery(SITESETTINGS, $fields);

		if (!isset($rs[2]))
		{
			if (!$rs[1]==0)
			{
				while ($res = mysql_fetch_array($rs[0]))
				{
					$settingsarray = array(0 => $res['site_name'], 1 => $res['site_status'], 2 => $res['subcats'], 3 => $res['useowneremail'], 4 => $res['alternateemail'], 5 => $res['resizeimgs'], 6 => $res['site_description']);
				}
				$ssfound = 1;

			}
			else
			{
				unset($ssfound);
			}
			unset($_SESSION['seriouserror']);
		}
		else
		{
			error_push('Error Retrieving Site Settings! Please contact support.');
			$_SESSION['seriouserror'] = 1;
			unset($ssfound);
		}

		unset($fields);
		unset($rs);
		unset($res);
		unset($DBA);

		return $settingsarray;
	}

	function getSiteOwner()
	{
		global $sofound;

		$DBA = new DatabaseInterface();
		$Owner = new User();

		$fields = array(0 => 'id');
		$idfields = array(0 => 'auth_level');
		$idvals = array(0 => '1');

		$rs = $DBA->selectQuery(DBUSERTABLE, $fields, $idfields, $idvals);

		if (!isset($rs[2]))
		{
			if (!$rs[1] == 0)
			{
				while ($res = mysql_fetch_array($rs[0]))
				{
					$Owner->ID = $res['id'];
					$Owner->getUser();

					$ownerinfo = array(0 => $Owner->Name, 1 => $Owner->Email);
				}
				$sofound = 1;
			}
			else
			{
				unset($sofound);
			}
			if (!isset($_SESSION['seriouserror']))
			{
				unset($_SESSION['seriouserror']);
			}
		}
		else
		{
			session_push('<li><img src="images/error.png" alt="Serious Error Warning" /></li>');
			session_push('<li>Error Retrieving Site Owner Record! Please contact support.</li>');

			$_SESSION['seriouserror'] = 2;

			define(SITE_OWNER, null);
			define(SITE_OWNER_EMAIL, null);
			unset($sofound);
		}

		unset($Owner);
		unset($fields);
		unset($idfields);
		unset($idvals);
		unset($rs);
		unset($res);
		unset($DBA);

		return $ownerinfo;
	}

	function createSiteSettings ()
	{
		$settingsarray = getSiteSettings();
		$ownerarray = getSiteOwner();

		if (((!isset($ownerarray[0])) || (!isset($settingsarray[0]))) && (!isset($_SESSION['seriouserror'])))
		{
			define(SETTINGS, '1');
		}
		else
		{
			define(SETTINGS, null);
		}

		define(SITE_STATUS, $settingsarray[1]);
		define(SUBCATEGORIES, $settingsarray[2]);
		define(USEOEMAIL, $settingsarray[3]);
		define(SITE_OWNER, $ownerarray[0]);
		define(RESIZEIMGS, $settingsarray[5]);
		define(SITE_DESCRIPTION, $settingsarray[6]);
		if (USEOEMAIL == 1)
		{
			define(SITE_OWNER_EMAIL, $ownerarray[1]);
		}
		else
		{
			define(SITE_OWNER_EMAIL, $settingsarray[4]);
		}
	}

    function encryption($strtohash)
    {
        $td = mcrypt_module_open('tripledes', '', 'ecb', '');
        $iv = mcrypt_create_iv (mcrypt_enc_get_iv_size($td), MCRYPT_RAND);
        mcrypt_generic_init($td, HASHKEY, $iv);
        $encrypted_data = mcrypt_generic($td, $strtohash);
        mcrypt_generic_deinit($td);
        mcrypt_module_close($td);

        return $encrypted_data;
    }

    function softencrypt($strtohash)
    {
        $td = mcrypt_module_open('des', '', 'ecb', '');
        $iv = mcrypt_create_iv (mcrypt_enc_get_iv_size($td), MCRYPT_RAND);
        mcrypt_generic_init($td, SOFTHASHKEY, $iv);
        $encrypted_data = mcrypt_generic($td, $strtohash);
        mcrypt_generic_deinit($td);
        mcrypt_module_close($td);

        return $encrypted_data;
    }

    function decryption($strtounhash)
    {
        $td = mcrypt_module_open('des', '', 'ecb', '');
        $iv = mcrypt_create_iv (mcrypt_enc_get_iv_size($td), MCRYPT_RAND);
        mcrypt_generic_init($td, SOFTHASHKEY, $iv);
        $unencrypted_data = mdecrypt_generic($td, $strtounhash);
        mcrypt_generic_deinit($td);
        mcrypt_module_close($td);

        return $unencrypted_data;
    }

	function cleanString ($str)
	{
		$str = str_replace('$nbsp;', '', $str);
		if (!get_magic_quotes_gpc)
		{
			$str = addslashes($str);
		}

		return $str;
	}

    function smart_chop($str, $length, $link = null, $minword = 3, $class = null, $link_text = null)
    {
        $sub = '';
        $len = 0;
        $wordcounter = 0;
        foreach (explode(' ', $str) as $word)
        {
            $wordcounter += 1;
            $part = (($sub != '') ? ' ' : '') . $word;
            $sub .= $part;
            $len += strlen($part);

            if (($wordcounter > $minword) && (strlen($sub) >= $length))
            {
                break;
            }
        }

        if (isset($link))
        {
            return $sub . (($len < strlen($str)) ?' <a href="' . $link . '" title="Read More"' . ($class ? 'class="' . $class . '"' : '') . '>' . ($link_text ? $link_text : '...') . '</a>' : '');
        }
        else
        {
            return $sub . (($len < strlen($str)) ? ($link_text ? $link_text : ' ...') : '');
        }
    }

    function prevent_inject($string)
    {
        $DBA = new DatabaseInterface();
        $DBA->dbConnect();
        $string = mysql_real_escape_string($string);
        unset($DBA);

        return ($string);

    }

    function extra_strip_tags_attributes($sSource, $aAllowedTags = array(), $aDisabledAttributes = array('onabort', 'onactivate', 'onafterprint', 'onafterupdate', 'onbeforeactivate', 'onbeforecopy', 'onbeforecut', 'onbeforedeactivate', 'onbeforeeditfocus', 'onbeforepaste', 'onbeforeprint', 'onbeforeunload', 'onbeforeupdate', 'onblur', 'onbounce', 'oncellchange', 'onchange', 'onclick', 'oncontextmenu', 'oncontrolselect', 'oncopy', 'oncut', 'ondataavaible', 'ondatasetchanged', 'ondatasetcomplete', 'ondblclick', 'ondeactivate', 'ondrag', 'ondragdrop', 'ondragend', 'ondragenter', 'ondragleave', 'ondragover', 'ondragstart', 'ondrop', 'onerror', 'onerrorupdate', 'onfilterupdate', 'onfinish', 'onfocus', 'onfocusin', 'onfocusout', 'onhelp', 'onkeydown', 'onkeypress', 'onkeyup', 'onlayoutcomplete', 'onload', 'onlosecapture', 'onmousedown', 'onmouseenter', 'onmouseleave', 'onmousemove', 'onmoveout', 'onmouseover', 'onmouseup', 'onmousewheel', 'onmove', 'onmoveend', 'onmovestart', 'onpaste', 'onpropertychange', 'onreadystatechange', 'onreset', 'onresize', 'onresizeend', 'onresizestart', 'onrowexit', 'onrowsdelete', 'onrowsinserted', 'onscroll', 'onselect', 'onselectionchange', 'onselectstart', 'onstart', 'onstop', 'onsubmit', 'onunload'))
    {
        if (empty($aDisabledAttributes)) return strip_tags($sSource, implode('', $aAllowedTags));

        return preg_replace('/<(.*?)>/ie', "'<' . preg_replace(array('/javascript:[^\"\']*/i', '/(" . implode('|', $aDisabledAttributes) . ")[ \\t\\n]*=[ \\t\\n]*[\"\'][^\"\']*[\"\']/i', '/\s+/'), array('', '', ' '), stripslashes('\\1')) . '>'", strip_tags($sSource, implode('', $aAllowedTags)));
    }

    function strip_fuck($content)
    {
        $badwords = array("asshole","ass","bitch","bastard","cunt","dick","dike","dildo","fuck","gay","hoe","nigger","pussy","slut","whore","god damn","goddamn","asshole","twat","whore","wank","cock","arse","arsehole","shit");
        $wordreplace = array ("!","@","#","%","^","&","*","~");

        $count = count($badwords);
        $countfilter = count($wordreplace);
       // Loop through the badwords array
              for ($n = 0; $n < $count; ++$n, next ($badwords)) {
           //Create random replace characters
           $x = 2;
           $y = rand(3,5);
           $filter = "";
              while ($x<="$y") {
            $f = rand(0,$countfilter);
            $filter .="$wordreplace[$f]";
              $x++;
              }

           //Search for badwords in content
           $search = "$badwords[$n]";
           $content = preg_replace("'$search'i","<i>$filter</i>",$content);

              }
       return $content;
    }

	function check_template ($filename)
	{
		$filename = SERVER_ROOT . TPLFOLDER . $filename;

		if (file_exists($filename))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

    function getSecurityLevels()
    {
        $DBA = new DatabaseInterface();

        $fields = array(0 => 'id', 1 => 'name');
        $orderby = 'id DESC';

        $rs = $DBA->selectQuery(DBSECURITYLVLTABLE, $fields, null, null, $orderby);

        if (!isset($rs[2]))
        {
            while($res = mysql_fetch_array($rs[0]))
            {
                $sec = array('id' => $res['id'], 'name' => $res['name']);

                if (isset($seclevs[0]))
                {
                    array_push($seclevs, $sec);
                }
                else
                {
                    $seclevs[0] = $sec;
                }

                unset($sec);
            }
        }

        unset($DBA);
        unset($fields);

        return $seclevs;
    }

    function getpagename($str)
    {
        $argarray = explode("=", $str);
        $ret = $argarray[1];
        return $ret;
    }

    function create_menu($currpagename='index')
    {
        unset($navlist);
        $navlist = '<li id="index"';
        if($currpagename=="index")
        {
            $navlist .= ' class="current"';
        }
        $navlist.='><a href="' . SITE_PATH . '" title="' . ucwords(SITE_NAME) . ' Index Page"><span>Index</span></a></li>';

        $DBA = new DatabaseInterface();

        $fields = array(0 => 'name');
        $orderby = 'mmorder';
        $rs = $DBA->selectQuery(DBMENUTABLE, $fields, null, null, $orderby);

        if (!isset($rs[2]))
        {
            while ($res = mysql_fetch_array($rs[0]))
            {
                if ($res['name'] != 'index')
                {
                    $pagename = $res['name'];
                    $pagepath = SITE_PATH . $res['name'] . '/';
                    if ($pagename == $currpagename)
                    {
                        $class='current';
                    }

                    $navlist .= '<li id="' . $pagename . '"';
                    if (isset($class))
                    {
                        $navlist .= ' class="' . $class . '"';
                        unset($class);
                    }
                    $navlist.='><a href="'. $pagepath . '" title="' . ucwords(SITE_NAME) . ' ' . str_replace('_', ' ',ucwords($pagename)) . '"><span>' . str_replace('_', ' ',ucwords($pagename)) . '</span></a></li>';
                }

            }
        }

        unset($res);
        unset($rs);
        unset($fields);
        unset($DBA);

        $catlist = getCategories('', 1);
        unset($categorylist);

        if (isset($catlist))
        {
          foreach ($catlist as $cat)
          {
              if (($cat['parentid'] == 0) && ($cat['dispmm'] == 1))
              {
                  $pagename = strtolower($cat['name']);
                  $pagepath = SITE_PATH . 'category/' . $pagename . '/';
                  $categorylist .= '<li id="' . $pagename . '"><a href="'. $pagepath . '" title="' . ucwords(SITE_NAME) . ' ' . str_replace('_', ' ',ucwords($pagename)) . '"><span>' . str_replace('_', ' ',ucwords($pagename)) . '</span></a></li>';
              }
          }
        }

        if(!isset($categorylist))
        {
            $navlist .= $categorylist;
        }

        return $navlist;
    }

    function update_app($sitename=SITE_NAME)
    {
        $FN = SERVER_ROOT . 'App.kfc';
        $fh = fopen($FN, 'r+') or ($failed=true);
        if (isset($failed))
        {
            return (2);
        }

        //read contents to search
        $appstr = fread($fh, filesize($FN));

        fclose($fh);

        $replace_str = "define('SITE_NAME','" . SITE_NAME . "');";
        $replace_with = "define('SITE_NAME','" . $sitename . "');";

        $appstr=str_replace($replace_str,$replace_with,$appstr);

        $FN = SERVER_ROOT . 'App.kfc';
        $fh = fopen($FN, 'w') or ($failed2=true);

        if (isset($failed2))
        {
            return (3);
        }

        fwrite($fh,$appstr);

        fclose($fh);

        return (1);
    }

    function create_sitemap($type='html')
    {
        if ($type=="html")
        {
            unset($maplist);
            $pagelist = '<li><a href="' . SITE_PATH . '" title="' . ucwords(SITE_NAME) . ' Home Page">Home</a></li>';

            $DBA = new DatabaseInterface();
            $fields = array(0 => 'name', 1 => 'type');
            $orderby = 'mmorder';
            $rs = $DBA->selectQuery(DBMENUTABLE, $fields, null, null, $orderby);
            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    if ($res['name'] != 'index')
                    {
                        $pagename = $res['name'];
                        $pagepath = SITE_PATH . $res['name'] . '/';

                        $pagelist .= '<li class="main-page"><a href="'. $pagepath . '" ' . ($res['type']=='s' ? ' class="heading"' : '') . 'title="' . ucwords(SITE_NAME) . ' ' . str_replace('_', ' ',ucwords($pagename)) . '">' . str_replace('_', ' ',ucwords($pagename)) . ($res['type'] == 's' ? '<img src="' . SITE_PATH . 'images/more.jpg" alt="More..." />' : '') . '</a>';
                        if ($res['type'] == 's')
                        {
                            //stick the section items in a dropdown list
                            $secid = getSectionID($pagename);
                            $itemlist = getSectionItemsList($secid);
                            $pagelist .= '<ul class="drop">';

                            foreach ($itemlist as $item)
                            {
                                $path = SITE_PATH . $pagename . '/' . $item['dbid'] . '/';
                                switch($pagename)
                                {
                                    case "testimonials":
                                        $value = $item['testimonial-author'];
                                        break;
                                    case "news":
                                        $value = $item['news-title'];
                                        break;
                                    case "catalogue":
                                        $value = $item['catalogue-name'];
                                        break;
                                }
                                $pagelist .= '<li class="section-page"><a href="'. $path . '" title="' . ucwords($value) . '">' . ucwords($value) . '</a></li>';
                            }

                            $pagelist .= '</ul>';
                        }
                        $pagelist .= '</li>';
                    }
                }
            }

            unset($res);
            unset($rs);
            unset($fields);
            unset($DBA);

            return '<ul id="accordion">' . $pagelist . '</ul>';
        }
    }

    function check_login($email=null,$password=null,$ref)
    {
        if (isset($_COOKIE['TKB']))
        {
            //check to see if there is a banning cookie if yes reset it that way it lasts longer each time they come back.
            $_SESSION['tries'] = '3';
            checktries();
        }

        if ($_SESSION['user']['verify'] == '1')
        {
            $vertimerunout = $_SESSION['user']['verifytime'] + DEFAULTVERLENGTH;
            $time = time();
            if ($vertimeout = $time)
            {
                $ret = loginuser($_SESSION['user']['name'], $_SESSION['user']['password']);
            }
        }
        else
        {
            $password = strip_tags($password);
            $username = strip_tags($email);

            if (($username == '') || ($password == ''))
            {
                error_push('Login Error');
    			error_push('You have not entered all the required information for login');

                if ($username == '')
                {
                    error_push('You must enter a username!');
                }

                if ($password == '')
                {
    				error_push('You must enter a password!');
                }
            }
            else
            {
                $ret = loginuser($username, $password);
            }
        }
        header("location:".$ref);
    }

    function sendemail($address,$name,$type,$extrainf)
    {
        //grab template text for the right type
        switch ($type){
            case "registration":
                $title = REGISTRATION_SUCCESS_TITLE;
                $text = REGISTRATION_SUCCESS_EMAIL;
                $text = str_replace('<_USERNAME_>', $name, $text);
                break;
            case "carregistration":
                $title = CARREGISTRATION_SUCCESS_TITLE;
                $text = CARREGISTRATION_SUCCESS_EMAIL;
                $text = str_replace('<_USERNAME_>', $name, $text);
                break;
            case "carupdate":
                $title = CARUPDATE_SUCCESS_TITLE;
                $text = CARUPDATE_SUCCESS_EMAIL;
                $text = str_replace('<_USERNAME_>', $name, $text);
                break;
            case "ftfentry":
                $title = FTFREGISTRATION_SUCCESS_TITLE;
                $text = FTFREGISTRATION_SUCCESS_EMAIL;
                $text = str_replace('<_USERNAME_>', $name, $text);
                break;
            case "OrderUpdate":
                $title = 'Order Update | Reference ' . $extrainf['order-ref'];
                $text = ORDER_UPDATE_MAIL;
                $text = str_replace('<_USERNAME_>', $name, $text);
                $text = str_replace('<_ORDERREF_>', $extrainf['order-ref'], $text);
                if ($extrainf['status'] == 'X')
                {
                    $text = str_replace('<_EXTRA_TEXT_>', ' and has been marked as refused.', $text);   
                }
                else
                {
                    $text = str_replace('<_EXTRA_TEXT_>', ' and is due for ' . $extrainf['del-col']  . ' at ' . $extrainf['readytime'] . '.', $text);
                }
                $text = str_replace('<_VIEW_LINK_>', $extrainf['viewlink'], $text);
                $text = str_replace('<_STORE_NAME_>', $extrainf['storename'], $text);
                break;
        }

        //replace variables with passed ones
        //create headers
        $headers = "From: " . SITE_OWNER_EMAIL . "\r\nReply-to: " . $inputs['email'];
        //send mail
        mail ($address, $title, $text ,$headers);
    }

    function show_menus($type)
    {
        $supplierlist = array();
        $menulist = array();

        if ($type != 'A')
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'id', 1 =>'name', 2 => 'thumbnail');
            $idfields = array(0 => 'type', 1 => 'name');
            $idvals = array(0 => $type, 1 => '!none');
            $orderby = 'name';

            $rs = $DBA->selectQuery(DBSUPPLIERSTABLE, $fields, $idfields, $idvals, $orderby);

            if ((!isset($rs[2])) && (!$rs[1]==0))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    echo '<div class="menu">';
                    echo '<div class="suppliername">' . $res['name'] . '</div>';
                    echo '<div class="thumbnail"><img src="' . SITE_PATH . 'images/supplier-thumbs/' . $res['thumbnail'] . '" alt="' . $res['name'] . '" /></div>';
                    $DBA2 = new DatabaseInterface();
                    $fields2 = array(0 => 'smallimg', 1 => 'largeimg');
                    $idfields2 = array(0 => 'supplier_id');
                    $idvals2 = array(0 => $res['id']);
                    $orderby2 = 'id';

                    $count = 1;
                    $rs2 = $DBA2->selectQuery(DBMENUSTABLE, $fields2, $idfields2, $idvals2, $orderby2);

                    if ((!isset($rs2[2])) && (!$rs2[1]==0))
                    {
                        echo '<div class="mcont">';
                        while ($res2 = mysql_fetch_array($rs2[0]))
                        {
                            echo '<div class="menulinks"><span class="menuname">Menu ' . $count . ' - </span><a href="' . SITE_PATH . TEMP_MENU_IMAGES . $res2['smallimg'] . '" title="' . $res['name'] . ' Online Menu ' . $count . '" class="lightbox"><img src="' . SITE_PATH . 'images/view.png" alt="View Menu" /></a><a class="button" href="' . SITE_PATH . TEMP_MENU_IMAGES . 'large/' . $res2['largeimg'] . '" title="' . $res['name'] . ' Online Menu ' . $count . ' - Click this to download"><img src="' . SITE_PATH . 'images/download.png" alt="Download Menu" /></a></div>';
                            $count+=1;
                        }
                        echo '</div>';
                    }
                    unset($fields2, $idfields2, $idvals2, $orderby2, $rs2, $res2, $DBA2);
                    echo '</div>';
                }
            }
            unset($rs,$fields,$idvals,$idfields,$DBA);
        }
        else
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'id', 1 => 'name', 2 => 'thumbnail');
            $idfields = array(0 => 'type', 1 => 'name');
            $idvals = array(0 => 'T', 1 => '!none');
            $orderby = 'name';

            echo '<h3>Takeaways</h3>';
            $rs = $DBA->selectQuery(DBSUPPLIERSTABLE, $fields, $idfields, $idvals, $orderby);

            if ((!isset($rs[2])) && (!$rs[1]==0))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    echo '<div class="menu">';
                    echo '<div class="suppliername">' . $res['name'] . '</div>';
                    echo '<div class="thumbnail"><img src="' . SITE_PATH . 'images/supplier-thumbs/' . $res['thumbnail'] . '" alt="' . $res['name'] . '" /></div>';
                    $DBA2 = new DatabaseInterface();
                    $fields2 = array(0 => 'smallimg', 1 => 'largeimg');
                    $idfields2 = array(0 => 'supplier_id');
                    $idvals2 = array(0 => $res['id']);
                    $orderby2 = 'id';

                    $count = 1;
                    $rs2 = $DBA2->selectQuery(DBMENUSTABLE, $fields2, $idfields2, $idvals2, $orderby2);

                    if ((!isset($rs2[2])) && (!$rs2[1]==0))
                    {
                        echo '<div class="mcont">';
                        while ($res2 = mysql_fetch_array($rs2[0]))
                        {
                            echo '<div class="menulinks"><span class="menuname">Menu ' . $count . ' - </span><a href="' . SITE_PATH . TEMP_MENU_IMAGES . $res2['smallimg'] . '" title="' . $res['name'] . ' Online Menu ' . $count . '" class="lightbox"><img src="' . SITE_PATH . 'images/view.png" alt="View Menu" /></a><a class="button" href="' . SITE_PATH . TEMP_MENU_IMAGES . 'large/' . $res2['largeimg'] . '" title="' . $res['name'] . ' Online Menu ' . $count . ' - Click this to download"><img src="' . SITE_PATH . 'images/download.png" alt="Download Menu" /></a></div>';
                            $count+=1;
                        }
                        echo '</div>';
                    }
                    unset($fields2, $idfields2, $idvals2, $orderby2, $rs2, $res2, $DBA2);
                    echo '</div>';
                }
            }
            unset($rs,$fields,$idvals,$idfields,$DBA);

            $DBA = new DatabaseInterface();
            $fields = array(0 => 'id', 1 =>'name', 2 => 'thumbnail');
            $idfields = array(0 => 'type', 1 => 'name');
            $idvals = array(0 => 'R', 1 => '!none');
            $orderby = 'name';

            echo '<h3>Restaurants</h3>';
            $rs = $DBA->selectQuery(DBSUPPLIERSTABLE, $fields, $idfields, $idvals, $orderby);

            if ((!isset($rs[2])) && (!$rs[1]==0))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    echo '<div class="menu">';
                    echo '<div class="suppliername">' . $res['name'] . '</div>';
                    echo '<div class="thumbnail"><img src="' . SITE_PATH . 'images/supplier-thumbs/' . $res['thumbnail'] . '" alt="' . $res['name'] . '" /></div>';
                    $DBA2 = new DatabaseInterface();
                    $fields2 = array(0 => 'smallimg', 1 => 'largeimg');
                    $idfields2 = array(0 => 'supplier_id');
                    $idvals2 = array(0 => $res['id']);
                    $orderby2 = 'id';

                    $count = 1;
                    $rs2 = $DBA2->selectQuery(DBMENUSTABLE, $fields2, $idfields2, $idvals2, $orderby2);

                    if ((!isset($rs2[2])) && (!$rs2[1]==0))
                    {
                        echo '<div class="mcont">';
                        while ($res2 = mysql_fetch_array($rs2[0]))
                        {
                            echo '<div class="menulinks"><span class="menuname">Menu ' . $count . ' - </span><a href="' . SITE_PATH . TEMP_MENU_IMAGES . $res2['smallimg'] . '" title="' . $res['name'] . ' Online Menu ' . $count . '" class="lightbox"><img src="' . SITE_PATH . 'images/view.png" alt="View Menu" /></a><a class="button" href="' . SITE_PATH . TEMP_MENU_IMAGES . 'large/' . $res2['largeimg'] . '" title="' . $res['name'] . ' Online Menu ' . $count . ' - Click this to download"><img src="' . SITE_PATH . 'images/download.png" alt="Download Menu" /></a></div>';
                            $count+=1;
                        }
                        echo '</div>';
                    }
                    unset($fields2, $idfields2, $idvals2, $orderby2, $rs2, $res2, $DBA2);
                    echo '</div>';
                }
            }
            unset($rs,$fields,$idvals,$idfields,$DBA);
        }
    }

    function create_slideshow()
    {
        $DBA = new DatabaseInterface();
        $fields = array(0 => 'image', 1 => 'link', 2=> 'id_name');
        $orderby = 'show_order';

        $rs = $DBA->selectQuery(DBSLIDESHOWTABLE, $fields, null, null, $orderby);
        if ($rs[2] == 0)
        {
            while ($res = mysql_fetch_array($rs[0]))
            {
                $image = $res['image'];
                $imagepath = SITE_PATH . SSIMAGESPATH . $res['image'];
                $name = $res['id_name'];
                if (!$res['link'] == "")
                {
                    $link = SITE_PATH . $res['link'];
                }
                else
                {
                    unset($link);
                }
                echo '<li>';
                if (isset($link))
                {
                    echo '<a href="' . $link . '" title="' . $name . '">';
                }
                echo '<img src="' . $imagepath . '" alt="' . $name . '">';
                if (isset($link))
                {
                    echo '</a>';
                }
                echo '</li>';
            }
        }
    }

    function find_distance($point1_lat, $point1_long)
    {
        $return = array();

        $sql = 'SELECT id, ( 3959 * acos( cos( radians(' . $point1_lat . ') ) * cos( radians( lat ) ) * cos( radians( lng ) - radians(' . $point1_long . ') ) + sin( radians(' . $point1_lat . ') ) * sin( radians( lat ) ) ) ) AS distance FROM ' . DBSTORETABLE . ' HAVING distance < 5 ORDER BY distance';
        $DBA = new DatabaseInterface();
        $rs = $DBA->runQuery($sql);

        if ((!isset($rs[2])) || ($rs[1] != 0))
        {
            while ($res = mysql_fetch_array($rs[0]))
            {
                array_push($return, array('storeid' => $res['id'], 'distance' => $res['distance']));
            }
        }
        elseif ($rs[1] == 0)
        {
            //no records found
            array_push($return, '<h2>No Records Found</h2><p>Sorry but there were no stores found near you.</p>');
        }
        else
        {
            //error somewhere in statement
            array_push($return, '<h2>Error</h2><p>Our apologies but there seems to have been an error in the search routine. Please try again and if you still have no luck <a href="' . SITE_PATH . 'contact_us/" title="Contact Us">let us know</a> or try again later.</p>');
        }

        return $return;
    }

    function create_random()
    {
        $str = '';
        $values = array('a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v' ,'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K' ,'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9');

        for ($i = 1; $i <= 25; $i+=1)
        {
            $num = rand(0, 61);
            $str .= $values[$num];
        }
        return $str;
    }

    function numtoword($num)
    {
        $values = array(0 => 'zero', 1 => 'one', 2 => 'two', 3 => 'three', 4 => 'four', 5 => 'five', 6 => 'six', 7 => 'seven', 8 => 'eight', 9 => 'nine', 10 => 'ten');
        return $values[$num];
    }

    function is_postcode($postcode) {
        $postcode = strtoupper($postcode);
        if(preg_match("/((GIR 0AA)|(TDCU 1ZZ)|(ASCN 1ZZ)|(BIQQ 1ZZ)|(BBND 1ZZ)"
    ."|(FIQQ 1ZZ)|(PCRN 1ZZ)|(STHL 1ZZ)|(SIQQ 1ZZ)|(TKCA 1ZZ)"
    ."|[A-PR-UWYZ]([0-9]{1,2}|([A-HK-Y][0-9]"
    ."|[A-HK-Y][0-9]([0-9]|[ABEHMNPRV-Y]))"
    ."|[0-9][A-HJKS-UW]) [0-9][ABD-HJLNP-UW-Z]{2})/", $postcode)) {
            return $postcode;
        } else {
            return FALSE;
        }
    }

    function get_file($add)
    {
        $ch = curl_init();
        $timeout = 5;
        curl_setopt($ch,CURLOPT_URL,$add);
        curl_setopt($ch,CURLOPT_RETURNTRANSFER,1);
        curl_setopt($ch,CURLOPT_CONNECTTIMEOUT,$timeout);
        $data = curl_exec($ch);
        curl_close($ch);
        return $data;
    }

    function geoLocate($id = null, $pcode = null, $house = null, $add2 = null, $street = null, $add4 = null, $town = null, $county = null)
    {
         $key = GAPI_KEY;
         if ((isset($house)) && ($house != ""))
         {
            //house number set
            $url = $house;
            $url2 = $house;
         }

         if ((isset($add2)) && ($add2 != ""))
         {
            if ($url != '')
            {
                $url .= '+';
                $url2 .= '+';
            }
            $url .= $add2;
            $url2 .= $add2;
         }

         if ((isset($street)) && ($street != ""))
         {
            //street set
            if ($url != '')
            {
                $url .= '+';
                $url2 .= '+';
            }
            $url .= $street;
            $url2 .= $street;
         }

         if ((isset($add4)) && ($add4 != ""))
         {
            if ($url != '')
            {
                $url .= '+';
                $url2 .= '+';
            }
            $url .= $add4;
            $url2 .= $add4;
         }

         if ((isset($town)) && ($town != ""))
         {
            //town set
            if ($url != '')
            {
                $url .= '+';
            }
            $url .= $town;
         }

         if ((isset($county)) && ($county != ""))
         {
            //county set
            if ($url != '')
            {
                $url .= '+';
            }
            $url .= $county;
         }

         if ((isset($pcode)) && ($pcode != ""))
         {
            //pcode set
            if ($url != '')
            {
                $url .= '+';
                $url2 .= '+';
                $justp = false;
            }
            else
            {
                $justp = true;    
            }
            $url .= $pcode;
            $url2 .= $pcode;
         }

         // Loop through each row, submit HTTP request, output coordinates
         $mapaddress = urlencode($url);
         $mapaddress2 = urlencode($url2);
        
         // Desired address
         $url = "http://maps.google.com/maps/geo?q=$mapaddress&output=xml&key=$key";
         $url2 = "http://maps.google.con/maps/geo?q=$mapaddress2&output=xml&key=$key";

         // Retrieve the URL contents
         $page = get_file($url);
         
         // Parse the returned XML file
         $xml = new SimpleXMLElement($page);

         if ($xml->Response->Status->code != "200")
         {
             //error or not found try again with just a bit less info.
             $page2 = get_file($url2);
             if ((isset($page2)) && ($page2 != ""))
             {
                 $xml2 = new SimpleXMLElement($page2);
                 if ($xml2->Response->Status->code != "200")
                 {
                     $accuracy = 0;
                 }
                 else
                 {
                     $accuracy = (int)$xml2->Response->Placemark->AddressDetails->attributes()->Accuracy;
                 }
            }
            else
            {
                $accuracy = 0;
            }
         }
         else
         {
            $accuracy = (int)$xml->Response->Placemark->AddressDetails->attributes()->Accuracy;
         }
            
    
         if (($accuracy < 6) && ($justp == false))
         {
             unset($mapcoords);
             return false;
         }
         else
         {
            // Parse the coordinate string
             list($longitude, $latitude, $altitude) = explode(",", $xml->Response->Placemark->Point->coordinates);

             $mapcoords = array();
             $mapcoords['long'] = $longitude;
             $mapcoords['lat'] = $latitude;

             if (isset($id))
             {
                 $DBA = new DatabaseInterface();
                 $fields = array(0 => 'geolat', 1 => 'geolong');
                 $fieldvals = array(0 => $mapcoords['lat'], 1 => $mapcoords['long']);
                 $idfields = 'id';
                 $idvals = $id;

                 $rs = $DBA->updateQuery(DBSTORETABLE, $fields, $fieldvals, $idfields, $idvals);

                 unset ($rs, $fields, $idfields, $fieldvals, $idvals, $DBA);
             }

             return($mapcoords);
        }
    }

    function reverse_geolocate($lat, $long)
    {
        $key = GAPI_KEY;

        $url = 'http://maps.google.com/maps/geo?q=' . $lat . ',' . $long . '&output=json&sensor=true_or_false&key=' . $key;
        // make the HTTP request
        $data = @file_get_contents($url);
        // parse the json response
        $jsondata = json_decode($data,true);
        // if we get a placemark array and the status was good, get the address
        if(is_array($jsondata)&& $jsondata['Status']['code']==200)
        {
          $town = $jsondata['Placemark'][0]['AddressDetails']['Country']['Locality']['LocalityName'];
        }
        return ($town);
    }

   function distance_apart($pairing1, $pairing2, $miles = true)
   {
      $pi80 = M_PI / 180;
      $pairing1['Lat'] *= $pi80;
      $pairing1['Long'] *= $pi80;
      $pairing2['lat'] *= $pi80;
      $pairing2['long'] *= $pi80;

      $r = 6372.797; // mean radius of Earth in km
      $dlat = $pairing2['lat'] - $pairing1['Lat'];
      $dlng = $pairing2['long'] - $pairing1['Long'];

      $a = sin($dlat / 2) * sin($dlat / 2) + cos($pairing1['Lat']) * cos($pairing2['lat']) * sin($dlng / 2) * sin($dlng / 2);
      $c = 2 * atan2(sqrt($a), sqrt(1 - $a));
      $km = $r * $c;
      return ($miles ? ($km * 0.621371192) : $km);
  }

  function cmp_matchcount($a, $b)
  {
    if ($a['matchcount'] == $b['matchcount'])
    {
        return 0;
    }
    return ($a['matchcount'] > $b['matchcount']) ? -1 : 1;
  }

  function cmp_distance($a, $b)
  {
        if ($a['distancefromcust'] == $b['distancefromcust'])
        {
            return 0;
        }
        return ($a['distancefromcust'] < $b['distancefromcust']) ? -1 : 1;
  }

  function cmp_rating($a, $b)
  {
    if ($a['rating'] == $b['rating'])
    {
        return 0;
    }
    return ($a['rating'] > $b['rating']) ? -1 : 1;
  }

  function cmp_expires($a, $b)
  {
    if ($a['expires'] == $b['expires'])
    {
        return 0;
    }
    return ($a['expires'] > $b['expires']) ? -1 : 1;
  }
  /**
  * Create a 2D array from a CSV string
  *
  * @param mixed $data 2D array
  * @param string $delimiter Field delimiter
  * @param string $enclosure Field enclosure
  * @param string $newline Line seperator
  * @return
  */
  function parsecsv($data, $delimiter = ',', $enclosure = '"', $newline = "\n"){
      $pos = $last_pos = -1;
      $end = strlen($data);
      $row = 0;
      $quote_open = false;
      $trim_quote = false;

      $return = array();

      // Create a continuous loop
      for ($i = -1;; ++$i){
          ++$pos;
          // Get the positions
          $comma_pos = strpos($data, $delimiter, $pos);
          $quote_pos = strpos($data, $enclosure, $pos);
          $newline_pos = strpos($data, $newline, $pos);

          // Which one comes first?
          $pos = min(($comma_pos === false) ? $end : $comma_pos, ($quote_pos === false) ? $end : $quote_pos, ($newline_pos === false) ? $end : $newline_pos);

          // Cache it
          $char = (isset($data[$pos])) ? $data[$pos] : null;
          $done = ($pos == $end);

          // It it a special character?
          if ($done || $char == $delimiter || $char == $newline){

              // Ignore it as we're still in a quote
              if ($quote_open && !$done){
                  continue;
              }

              $length = $pos - ++$last_pos;

              // Is the last thing a quote?
              if ($trim_quote){
                  // Well then get rid of it
                  --$length;
              }

              // Get all the contents of this column
              $return[$row][] = ($length > 0) ? str_replace($enclosure . $enclosure, $enclosure, substr($data, $last_pos, $length)) : '';

              // And we're done
              if ($done){
                  break;
              }

              // Save the last position
              $last_pos = $pos;

              // Next row?
              if ($char == $newline){
                  ++$row;
              }

              $trim_quote = false;
          }
          // Our quote?
          else if ($char == $enclosure){

              // Toggle it
              if ($quote_open == false){
                  // It's an opening quote
                  $quote_open = true;
                  $trim_quote = false;

                  // Trim this opening quote?
                  if ($last_pos + 1 == $pos){
                      ++$last_pos;
                  }

              }
              else {
                  // It's a closing quote
                  $quote_open = false;

                  // Trim the last quote?
                  $trim_quote = true;
              }

          }

      }

      return $return;
  }

  function get_guide($area)
  {
        $DBA = new DatabaseInterface();
        $fields = array(0 => 'id');
        $idfields = array(0 => 'name');
        $idvals = array(0 => ucwords($area));
        $rs = $DBA->selectQuery(DBTOWNSTABLE, $fields, $idfields, $idvals);

        if (!isset($rs[2]))
        {
            if ($rs[1] == 0)
            {
                //$guidecode = '<p>Our apologies but there are no guides listed for your area at present. If you would like to point us in your direction then please <a href="' . SITE_PATH . '"contact_us/" title="Contact Us">contact us</a> and tell us the area you are in.</p>';
                $end = 1;
            }
            else
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $area_id = $res['id'];
                }
            }
        }
        unset($rs, $res, $fields, $idfields, $idvals, $DBA);

        if ($end != 1)
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'title', 1 => 'idate', 2 => 'code');
            $idfields = array(0 => 'area');
            $idvals = array(0 => $area_id);
            $orderby = 'id desc';
            $limit = '1';

            $rs = $DBA->selectQuery(DBGUIDESTABLE, $fields, $idfields, $idvals, $orderby, $limit);
            if (!isset($rs[2]))
            {
                while($res = mysql_fetch_array($rs[0]))
                {
                    $guidecode = $res;
                }
            }
            unset ($rs, $res, $fields, $idfields, $idvals, $orderby, $limit, $DBA);
        }
        else
        {
            unset($guidecode);
        }

        return ($guidecode);
  }

  function get_flag($foodtype)
  {
        $DBA = new DatabaseInterface();
        $fields = array(0 => 'flag', 1 => 'name');
        $idfields = array(0 => 'id');
        $idvals = array(0 => $foodtype);

        $rs = $DBA->selectQuery(DBCUISINETYPESTABLE, $fields, $idfields, $idvals);

        if ((!isset($rs[2])) && ($rs[1] != 0))
        {
            while ($res = mysql_fetch_array($rs[0]))
            {
                $imgstr = '<img src="' . SITE_PATH . FLAG_PATH . $res['flag'] . '" alt="' . $res['name'] . '" />';
            }
        }
        return ($imgstr);
  }

  function get_rating($storeid)
  {
        $DBA = new DatabaseInterface();
        $fields = array(0 => 'sum(rating - 1) as total');
        $idfields = array(0 => 'store_id');
        $idvals = array(0 => $storeid);
        $groupby = 'store_id';

        $rs = $DBA->selectQuery(DBRATINGSTABLE, $fields, $idfields, $idvals, null, null, $groupby);
        if (!isset($rs[2]))
        {
            while ($res = mysql_fetch_array($rs[0]))
            {
                $total = $res['total'];
            }
        }

        unset ($rs, $res, $fields, $idfields, $idvals, $DBA);

        $DBA = new DatabaseInterface();
        $fields = array(0 => 'count(rating) as counted');
        $idfields = array(0 => 'store_id');
        $idvals = array(0 => $storeid);

        $rs = $DBA->selectQuery(DBRATINGSTABLE, $fields, $idfields, $idvals);

        if (!isset($rs[2]))
        {
            while ($res = mysql_fetch_array($rs[0]))
            {
                $counted = $res['counted'];
            }
        }
        unset ($rs, $res, $fields, $idfields, $idvals, $DBA);

        if ($counted != 0)
        {
            $avg = abs($total / $counted);
        }
        else
        {
            $avg = 0;
        }

        if ($avg == '')
        {
            $avg = 0;
        }
        return $avg;
  }

  function get_search_icon($pos)
  {
        switch($pos) {
            case 1:
                $icon = SITE_PATH . 'images/one.gif';
                break;
            case 2:
                $icon = SITE_PATH . 'images/two.gif';
                break;
            case 3:
                $icon = SITE_PATH . 'images/three.gif';
                break;
            case 4:
                $icon = SITE_PATH . 'images/four.gif';
                break;
            case 5:
                $icon = SITE_PATH . 'images/five.gif';
                break;
            case 6:
                $icon = SITE_PATH . 'images/six.gif';
                break;
            case 7:
                $icon = SITE_PATH . 'images/seven.gif';
                break;
            case 8:
                $icon = SITE_PATH . 'images/eight.gif';
                break;
            case 9:
                $icon = SITE_PATH . 'images/nine.gif';
                break;
            case 10:
                $icon = SITE_PATH . 'images/ten.gif';
                break;
        }
        return($icon);
  }

  function getmaxid($tablename, $increment=null)
  {
        $DBA = new DatabaseInterface();
        $fields = array(0 => 'max(id) as id');

        $rs = $DBA->selectQuery($tablename, $fields);
        if (!isset($rs[2]))
        {
            while ($res = mysql_fetch_array($rs[0]))
            {
                $id = $res['id'];
            }
        }

        if (($id == 0) || ($id == ''))
        {
            $id = 1;
        }
        else
        {
            $id += 1;
        }

        unset($fields, $rs, $res, $DBA);

        if (isset($increment))
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'id');
            $fieldvals = array(0 => $id);
            $rs = $DBA->updateQuery($tablename, $fields, $fieldvals);

            unset($rs, $fields, $fieldvals, $DBA);
        }

        return $id;
  }

  function getMaxVoucherNumber()
  {
        $DBA = new DatabaseInterface();
        $fields = array(0 => 'max(voucher_number) as numb');

        $rs = $DBA->selectQuery(DBVOUCHERNUMBERTABLE, $fields);
        if (!isset($rs[2]))
        {
            while ($res = mysql_fetch_array($rs[0]))
            {
                $numb = $res['numb'];
            }
        }

        if (($numb == 0) || ($numb == ''))
        {
            $numb = 1;
        }
        else
        {
            $numb += 1;
        }

        unset($fields, $rs, $res, $DBA);

        $DBA = new DatabaseInterface();
        $fields = array(0 => 'voucher_number');
        $fieldvals = array(0 => $numb);
        $rs = $DBA->updateQuery(DBVOUCHERNUMBERTABLE, $fields, $fieldvals);

        unset ($fields, $fieldvals, $rs, $DBA);
        return $numb;
  }

  function check_orphaned_cart($userid)
  {
        if (isset($_SESSION['basket_id']))
        {
            $Basket = new Basket();
            $Basket->setBasketVar('ID', $_SESSION['basket_id']);
            $Basket->setBasketVar('UserID', $userid);
            $Basket->getBasket('login');
            $Basket->updateBasketRecord();
        }
  }

  function getStoresbyTown($town)
  {
       $ids = array();
       $DBA = new DatabaseInterface();
       $fields = array(0 => 'store_id',1 => 'location_id');
       $idfields = array(0 => 'town');
       $idvals = array(0 => $town);

       $rs = $DBA->selectQuery(DBSTORELOCATIONTABLE, $fields, $idfields, $idvals);

       if (!isset($rs[2]))
       {
            while($res = mysql_fetch_array($rs[0]))
            {
                 $temp = array('store' => $res['store_id'], 'location' => $res['location_id']);
                 array_push($ids, $temp);
            }
       }

       unset($rs,$res,$fields,$idfields,$idvals);

       return($ids);
  }

  function textToImage($image, $x, $y, $color=null, $font, $text, $fontsize=20, $angle=0)
  {
       imagettftext($image, $fontsize, $angle, $x, $y, $color, $font, $text);
  }

  function saveDownloadDetails($uid, $assoc, $time, $type, $store_id, $dbid)
  {
        $mainid = getmaxid(DBDOWNLOADDETAILSTABLE);
        $DBA = new DatabaseInterface();
        $fields = array(0 => 'id', 1 => 'user_id', 2 => 'associated_id', 3 => 'download_time', 4 => 'type', 5 => 'store_id', 6 => 'dbid');
        $fieldvals = array(0 => $mainid, 1 => $uid, 2 => $assoc, 3 => $time, 4 => $type, 5 => $store_id, 6 => $dbid);
        $rs = $DBA->insertQuery(DBDOWNLOADDETAILSTABLE, $fields, $fieldvals);

        unset($mainid, $DBA, $rs, $fields, $idfields);
  }

  function getRatingCats($store_type)
  {
        $return = array();

        $DBA = new DatabaseInterface();
        $fields = array(0 => 'name', 1 => 'inputname', 2 => 'id');
        $idfields = array(0 => 'storetype');
        $idvals = array(0 => $store_type);

        $rs = $DBA->selectQuery(DBRATINGSCATSTABLE, $fields, $idfields, $idvals, null, null, null, null, 1);

        if (!isset($rs[2]))
        {
            while ($res = mysql_fetch_array($rs[0]))
            {
                array_push($return, $res);
            }
        }
        unset($fields, $idfields, $idvals, $res, $rs, $DBA);

        return($return);
  }

  function getStoreNamebyID($id)
  {
        $Store = new Store();
        $Store->getStore($id);
        return $Store->Name;
  }

  function checkAllowed($filesarray)
  {
      $allowedExtensions = array("jpg","jpeg","gif","png");
      foreach ($_FILES as $file)
      {
          if ($file['tmp_name'] > '')
          {
              if (!in_array(end(explode(".", strtolower($file['name']))), $allowedExtensions))
              {
                return(false);
                die();
              }
          }
      }
      return (true);
  }

  function createorderref()
  {
        $ref = 'ORD-';
        $id = getmaxid(DBREFERENCENOTABLE, 1);
        $ref .= $id;
        return($ref);
  }

  function getStorebyIMEI($imei)
  {
        $idret = array();
        $DBA = new DatabaseInterface();
        $fields = array(0 => 'store_id', 1 => 'location_id');
        $idfields = array(0 => 'imei');
        $idvals = array(0 => $imei);
        $limit = 1;

        $rs = $DBA->selectQuery(DBSTORELOCATIONTABLE, $fields, $idfields, $idvals,null,$limit);

        if (!isset($rs[2]))
        {
            while ($res = mysql_fetch_array($rs[0]))
            {
                array_push($idret, $res);
            }
        }

        unset($rs, $fields, $idfields, $idvals, $res, $DBA);
        return ($idret);
  }

  function gettimestampdifference($timestamp1, $timestamp2)
  {
        ## difference between the two in seconds
        $time_period = ( $timestamp2 - $timestamp1 );
        $time_period2 = $time_period;

        $days = 0;
        $hours = 0;
        $minutes = 0;
        $seconds = 0;

        $time_increments = array( 'days' => 86400,'hours' => 3600, 'minutes' => 60, 'seconds' => 1 );

        ## will hold our values for ( day, minute, hour, seconds )
        $time_span = array();

        ## cycle through time_increments
        while( list( $key, $value ) = each( $time_increments )) {
        $this_value = (int) ( $time_period / $value );
        $time_period = ( $time_period % $value );

        # save value
        $time_span[$key] = $this_value;
        }
        $time_span['time_period'] = $time_period2;

        return($time_span);
  }

  function get_hourdropdown($dayid, $ft, $dorn, $vals)
  {
        //what value do we have already for hours
        $temp = explode(' - ', $vals);
        switch ($ft) {
            case 'from':
                $ampmremover = substr($temp[0], -2);
                $temp[0] = str_replace($ampmremover, '', $temp[0]);
                $hour = explode(':', $temp[0]);
                $hour = $hour[0];
                break;
            case 'till':
                $ampmremover = substr($temp[1], -2);
                $temp[1] = str_replace($ampmremover, '', $temp[1]);
                $hour = explode(':', $temp[1]);
                $hour = $hour[0];
                break;
        }
        //get drop down for a day/night id = name of day
        for ($i = 1; $i <=12; $i++)
        {
            if (($hour == $i) && ($hour != 'Closed'))
            {
                $sel = 'selected="selected" ';
            }
            else
            {
                unset($sel);
            }
            $options .= '<option value="' . $i . '" '. $sel . '>' . ($i < 10 ? '0' : '') . $i . '</option>';
        }
        $id = $dayid . '-' . $dorn . '-' . $ft . '-hours';
        return ('<label for="' . $id .'">' . ucwords($ft) . '</label><select id="' . $id . '" name="' . $id . '">' . $options . '</select>');
  }

  function get_mindropdown($dayid, $ft, $dorn, $vals)
  {
        //what value do we have already for mins
        $temp = explode(' - ', $vals);
        switch ($ft) {
            case 'from':
                $ampmremover = substr($temp[0], -2);
                $temp[0] = str_replace($ampmremover, '', $temp[0]);
                $min = explode(':', $temp[0]);
                $min = $min[1];
                break;
            case 'till':
                $ampmremover = substr($temp[1], -2);
                $temp[1] = str_replace($ampmremover, '', $temp[1]);
                $min = explode(':', $temp[1]);
                $min = $min[1];
                break;
        }
        //get quarter hour intervals
        for ($i = 0; $i <= 45; $i+=15)
        {
            $mincheck = $i + '0';
            if (($min == ($mincheck)) && ($min != 'Closed'))
            {
                $sel = 'selected="selected" ';
            }
            else
            {
                unset($sel);
            }
            $options .= '<option value="' . $i . '" ' . $sel . '>' . ($i < 15? '0' : '') . $i . '</option>';
        }
        $id = $dayid . '-' . $dorn . '-' . $ft . '-mins';
        return ('<select id="' . $id . '" name="' . $id . '">' . $options . '</select>');
  }

  function get_ampmdropdown($dayid, $ft, $dorn, $vals)
  {
        //what value do we have already for mins
        $temp = explode(' - ', $vals);
        switch ($ft) {
            case 'from':
                $ampmremover = substr($temp[0], -2);
                break;
            case 'till':
                $ampmremover = substr($temp[1], -2);
                break;
        }
        for ($i = 0; $i <= 1; $i++)
        {
            switch ($i) {
                case 0:
                    $options = '<option value="am"';
                    if ($ampmremover == 'am')
                    {
                        $options .= ' selected="selected" ';
                    }
                    $options .= '>AM</option>';
                    break;
                case 1:
                    $options .= '<option value="pm"';
                    if ($ampmremover == 'pm')
                    {
                        $options .= ' selected="selected" ';
                    }
                    $options .= '>PM</option>';
                    break;
            }
        }
        $id = $dayid . '-' . $dorn . '-' . $ft . '-part';
        return ('<select id="' . $id . '" name="' . $id . '">' . $options . '</select>');
  }

  function getFeaturedImageTypes()
  {
        $returnarray = array();
        $DBA = new DatabaseInterface();
        $fields = array(0 => '*');
        $order = 'id asc';
        $rs = $DBA->selectQuery(DBFEATUREDIMAGETYPESTABLE, $fields, NULL, NULL, $order);
        if (!isset($rs[2]))
        {
            while ($res = mysql_fetch_array($rs[0]))
            {
                array_push($returnarray, $res);
            }
        }
        unset($DBA, $fields, $rs, $res);
        return $returnarray;
  }

  function getFeaturedImagesbyType($storeid, $locationid, $imagetype)
  {
        $imglist = array();
        $DBA = new DatabaseInterface();
        $fields = array(0 => '*');
        $idfields = array(0 => 'store_id', 1 => 'location_id', 2 => 'imagetype');
        $idvals = array(0 => $storeid, 1 => $locationid, 2 => $imagetype);
        $rs = $DBA->selectQuery(DBFEATUREDIMAGESTABLE, $fields, $idfields, $idvals);
        if (!isset($rs[2]))
        {
            while ($res = mysql_fetch_array($rs[0]))
            {
                array_push($imglist, $res);
            }
        }
        unset($DBA, $rs, $fields, $idfields, $idvals, $res);
        return $imglist;
  }

  function getFeaturedImageType($typeid)
  {
        $DBA = new DatabaseInterface();
        $fields = array(0 => 'name');
        $idfields = array(0 => 'id');
        $idvals = array(0 => $typeid);

        $rs = $DBA->selectQuery(DBFEATUREDIMAGETYPESTABLE, $fields, $idfields, $idvals);

        if (!isset($rs[2]))
        {
            while ($res = mysql_fetch_array($rs[0]))
            {
                $ret = $res['name'];
            }
        }
        unset($DBA, $rs, $fields, $idfields, $idvals, $res);
        return $ret;
  }

  function getCuisineTypes($id = null)
  {
        $temp = array();
        $DBA = new DatabaseInterface();
        unset($idfields, $idvals);
        $fields = array(0 => 'id', 1 => 'name', 2 => 'description');
        if (isset($id))
        {
            $idfields = array(0 => 'id');
            $idvals = array(0 => $id);
        }
        $rs = $DBA->selectQuery(DBCUISINETYPESTABLE, $fields, $idfields, $idvals);
        if (!isset($rs[2]))
        {
            while ($res = mysql_fetch_array($rs[0]))
            {
                array_push($temp, $res);
            }
        }
        unset($rs, $res, $fields, $DBA);
        return ($temp);
  }

  function compare_with_master($itemname, $itemdescription=null, $ctype)
  {
        //get categories from master with cusine type of $ctype place in array
        $mastercats = array();
        $DBA = new DatabaseInterface();
        $fields = array(0 => 'id');
        $idfields = array(0 => 'cuisine_type_id');
        $idvals = array(0 => $ctype);

        $rs = $DBA->selectQuery(DBOLMASTERMENUCATEGORYTABLE, $fields, $idfields, $idvals);

        if (!isset($rs[2]))
        {
            while ($res = mysql_fetch_array($rs[0]))
            {
                array_push($mastercats, $res['id']);
            }
        }

        $query = "select *, match(name) against('" . $itemname . "') as score from " . DBOLMASTERMENUITEMSTABLE . " where match(name) against('" . $itemname . "')";
        $DBA = new DatabaseInterface();
        $rs = $DBA->runQuery($query);

        while ($res = mysql_fetch_array($rs[0]))
        {
            if ($res['score'] > 1)
            {
                //check if its in a category from the master categories (filtered by cuisine type)
                if (in_array($res['category'], $mastercats))
                {
                    //good chance we've matched on the name, let it go through but return false, do not add to master but add to suppliers menu
                    unset($rs, $query, $DBA, $res);
                    return (false);
                }
            }
        }

        unset($rs, $query, $DBA, $res);
        $query = "select *, match(description) against('" . $itemdescription . "') as score from " . DBOLMASTERMENUITEMSTABLE . " where match(description) against('" . $itemdescription . "')";
        $DBA = new DatabaseInterface();
        $rs = $DBA->runQuery($query);

        while ($res = mysql_fetch_array($rs[0]))
        {
            if ($res['score'] > 1)
            {
                if (in_array($res['category'], $mastercats))
                {
                    //good chance we've matched on the description, let it go through but return false, do not add to master but add to suppliers menu
                    unset($rs, $query, $DBA, $res);
                    return (false);
                }
            }
        }

        unset($rs, $query, $DBA, $res);
        return true;
  }

  function checkExistingMasterCategory($catid, $cuisinetype)
  {
        $fields = array(0 => 'name');
        $idfields = array(0 => 'id');
        $idvals = array(0 => $catid);

        $DBA = new DatabaseInterface();
        $rs = $DBA->selectQuery(DBOLMENUCATEGORYTABLE, $fields, $idfields, $idvals);

        if (!isset($rs[2]))
        {
            while ($res = mysql_fetch_array($rs[0]))
            {
                $name = $res['name'];
            }
        }

        unset($fields, $idfields, $idvals, $rs, $res, $DBA);

        unset($nid);

        $query = "select *, match(name) against('" . $name . "') as score from " . DBOLMASTERMENUCATEGORYTABLE . " where match(name) against('" . $name . "') and cuisine_type_id='" . $cuisinetype . "'";
        $DBA = new DatabaseInterface();
        $rs = $DBA->runQuery($query);

        while ($res = mysql_fetch_array($rs[0]))
        {
            if ($res['score'] > $leadscore)
            {
                $nid = $res['id'];
                $leadscore = $res['score'];
            }
        }
        unset($rs, $query, $DBA, $res);
        if (isset($nid))
        {
            return $nid;
        }
        else
        {
            return false;
        }
  }

  function getMasterExtras()
  {
      $temparray = array();
      $DBA = new DatabaseInterface();
      $fields = array(0 => 'name', 1 => 'id');
      $orderby = 'id';
      $rs = $DBA->selectQuery(DBMASTEREXTRACATEGORYTABLE, $fields, null, null, $orderby);

      if (!isset($rs[2]))
      {
          while ($res = mysql_fetch_array($rs[0]))
          {
              $temparrayone = array('catid' => $res['id'], 'category' => $res['name'], 'items' => array());

              $DBA2 = new DatabaseInterface();

              $fields2 = array(0 => '*');
              $idfields = array(0 => 'category');
              $idvals = array(0 => $res['id']);
              $orderby2 = 'id';
              $rs2 = $DBA2->selectQuery(DBMASTEREXTRAITEMSTABLE, $fields, $idfields, $idvals, $orderby);
              if (!isset($rs2[2]))
              {
                  $temparraytwo = array();
                  while ($res2 = mysql_fetch_array($rs2[0]))
                  {
                      array_push($temparraytwo, $res2);
                  }
              }
              $temparrayone['items'] = $temparraytwo;
              array_push($temparray, $temparrayone);
          }
      }

      return $temparray;
  }

  function getExtraName($id, $cat)
  {
      $DBA = new DatabaseInterface();
      $fields = array(0 => 'name');
      $idfields = array(0 => 'id', 1 => 'category');
      $idvals = array(0 => $id, 1 => $cat);

      $rs = $DBA->selectQuery(DBMASTEREXTRAITEMSTABLE, $fields, $idfields, $idvals);

      if (!isset($rs[2]))
      {
          while($res = mysql_fetch_array($rs[0]))
          {
              $name = $res['name'];
          }
      }
      unset($DBA, $fields, $idfields, $idvals, $rs, $res);

      return $name;
  }

  function checkExtraExists($id, $cat, $storeid, $locationid, $menuid, $itemid, $name)
  {
      $DBA = new DatabaseInterface();
      $fields = array(0 => 'count(*) as total');
      $idfields = array(0 => 'master_extra_id', 1 => 'master_extra_category', 2 => 'store_id', 3 => 'location_id', 4 => 'menu_id', 5 => 'item_id', 6 => 'name');
      $idvals = array(0 => $id, 1 => $cat, 2 => $storeid, 3 => $locationid, 4 => $menuid, 5 => $itemid, 6 => $name);

      $rs = $DBA->selectQuery(DBMENUITEMEXTRASTABLE, $fields, $idfields, $idvals);

      if (!isset($rs[2]))
      {
          while($res = mysql_fetch_array($rs[0]))
          {
              $icounter = $res['total'];
          }
      }
      
      unset($DBA, $fields, $idfields, $idvals, $rs, $res);
      if ($icounter > 0)
      {
          return true;
      }
      else
      {
          return null;
      }
  }

  function gettotalextracategories()
  {
      $DBA = new DatabaseInterface();
      $fields = array(0 => 'count(*) as total');
      $rs = $DBA->selectQuery(DBMASTEREXTRACATEGORYTABLE, $fields);
      if (!isset($rs[2]))
      {
          while ($res = mysql_fetch_array($rs[0]))
          {
              $total = $res['total'];
          }
      }
      else
      {
          $total = 0;
      }
      unset($DBA, $rs, $res, $fields);
      return $total;
  }

  function getTotalExtraItems($category_id)
  {
      $DBA = new DatabaseInterface();
      $fields = array(0 => 'count(*) as total');
      $idfields = array(0 => 'category');
      $idvals = array( 0=> $category_id);
      $rs = $DBA->selectQuery(DBMASTEREXTRAITEMSTABLE, $fields, $idfields, $idvals);

      if (!isset($rs[2]))
      {
          while($res = mysql_fetch_array($rs[0]))
          {
              $total = $res['total'];
          }
      }
      unset($DBA, $fields, $idfields, $idvals, $rs, $res);
      return $total;
  }

  function getMainTabs()
  {
      $tablist = array();
      $DBA = new DatabaseInterface();
      $fields = array(0 => 'id', 1 => 'name');
      $rs = $DBA->selectQuery(DBMAINTABSTABLE, $fields);

      if (!isset($rs[2]))
      {
          while($res = mysql_fetch_array($rs[0]))
          {
              array_push($tablist, $res);
          }
      }
      unset($rs, $res, $fields, $DBA);

      return($tablist);
  }

  function getMainTabName($tabid)
  {
      $DBA = new DatabaseInterface();
      $fields = array(0 => 'name');
      $idfields = array(0 => 'id');
      $idvals = array(0 => $tabid);

      $rs = $DBA->selectQuery(DBMAINTABSTABLE, $fields, $idfields, $idvals);

      if (!isset($rs[2]))
      {
          while($res = mysql_fetch_array($rs[0]))
          {
              $name = $res['name'];
          }
      }

      unset($rs, $res, $DBA, $fields, $idfields, $idvals);

      return($name);
  }

  function curl_get_contents($url) {
        // Initiate the curl session
        $ch = curl_init();
        // Set the URL
        curl_setopt($ch, CURLOPT_URL, $url);
        // Removes the headers from the output
        curl_setopt($ch, CURLOPT_HEADER, 0);
        // Return the output instead of displaying it directly
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        // Execute the curl session
        $output = curl_exec($ch);
        // Close the curl session
        curl_close($ch);
        // Return the output as a variable
        return $output;
  }

  function backgroundPost($url){
      $parts=parse_url($url);
     
      $fp = fsockopen($parts['host'],
              isset($parts['port'])?$parts['port']:80,
              $errno, $errstr, 30);
     
      if (!$fp) {
          return false;
      } else {
          $out = "POST ".$parts['path']." HTTP/1.1\r\n";
          //$out = "GET " .$parts['path']." HTTP/1.1\r\n";
          $out.= "Host: ".$parts['host']."\r\n";
          $out.= "Content-Type: application/x-www-form-urlencoded\r\n";
          $out.= "Content-Length: ".strlen($parts['query'])."\r\n";
          $out.= "Connection: Close\r\n\r\n";
          if (isset($parts['query'])) $out.= $parts['query'];
     
          fwrite($fp, $out);
          fclose($fp);
          return true;
      }
    }
?>
