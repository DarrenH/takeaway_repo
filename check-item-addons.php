<?php
	session_start();

	require_once('App.kfc');
    require_once(SERVER_ROOT . 'includes/Class_DatabaseInterface2.inc');
    require_once(SERVER_ROOT . 'includes/Class_Menu.inc');
    require_once(SERVER_ROOT . 'includes/Class_User.inc');
    require_once(SERVER_ROOT . 'includes/functions.inc');

    $newUser = checklogged();
    if (isset($newUser))
    {
        $user_id = $newUser->ID;
    }
    else
    {
        $user_id = 0;
    }

    $found = false;

    if ($_GET['mid']!='')
    {
    	$extrasarray = array();

        $itemid = $_GET['iid'];
        $menuid = $_GET['mid'];
        if (isset($_GET['mpdesc']))
        {
            $mpdesc = $_GET['mpdesc'];
        }
        else
        {
            unset($mpdesc);
        }

    	$Menu = new Menu();
    	$Menu->setMenuVar("ID", $menuid);
    	$Menu->setMenuVar("StoreID", 1);
    	$Menu->setMenuVar("LocationID", 0);

    	$extrasarray = $Menu->getItemExtras($itemid,null,$mpdesc);
    	if (count($extrasarray) >= 1)
    	{
    		$formstr = '<div class="modal-header">';
			$formstr .= '<h4 id="choicesextrasModalLabel">Please Choose any extras you require:</h4>';
			$formstr .= '</div>';
			$formstr .= '<div class="modal-body">';
    		$formstr .= '<form class="form-horizontal">';
    		$formstr .= '<legend>Please select from the following extras</legend>';
    		$formstr .= '<input type="hidden" name="menuid" id="menuid" value="' . $Menu->ID . '" />';
    		$formstr .= '<input type="hidden" name="storeid" id="storeid" value="' . $Menu->StoreID . '" />';
    		$formstr .= '<input type="hidden" name="locationid" id="locationid" value="' . $Menu->LocationID . '" />';
    		$formstr .= '<input type="hidden" name="itemid" id="itemid" value="' . $itemid . '" />';
    		if ((isset($_GET['mpdesc'])) && ($mpdesc != ""))
    		{
    			$formstr .= '<input type="hidden" name="mpdesc" id="mpdesc" value="' . $mpdesc . '" />';
    		}
            $counter = 0;
    		foreach ($extrasarray as $extra)
    		{
   				$formstr .= '<div class="control-group">';
    			$formstr .= '<label class="radio">';
    			$formstr .= '<input type="radio" name="extrasel" id="extrasel' . $counter . '" value="' . $extra['id'] . '" />';
    			$formstr .= $extra['name'] . ' Cost: ' . $extra['cost'];
    			$formstr .= '</label>';
   				$formstr .= '</div>';
                $counter++;
    		}
    		$formstr .= '</div>';
    		$formstr .= '<div class="modal-footer">';
    		$formstr .= '<button type="submit" formaction="' . SITE_PATH . 'choose-extra/" class="btn btn-success">Choose selected</button>';
			$formstr .= '<button class="btn btn-danger" data-dismiss="modal" aria-hidden="true">No Thanks</button>';
			$formstr .= '</div>';
    		$formstr .= '</form>';
            $found = true;
    	}
        else
        {
            //check for available choices
            $icat = $Menu->getItemCategory($itemid);
            $resp = $Menu->getItem($itemid, $icat);
            $chtype = $Menu->getItemChoiceType($itemid);
            if ($chtype!='') {
                $choiceparts = explode('/', $chtype);
                switch ($choiceparts[0]) {
                    case "X":
                        $mandatory = $resp['choice_mandatory'];
                        $choicedets = $Menu->getItemChoices($itemid,$mpdesc);
                        $type = 'xor';
                        if (count($choicedets) >= 1)
                        {
                            $formstr = '<div class="modal-header">';
                            $formstr .= '<h4 id="choicesextrasModalLabel">Please Choose:</h4>';
                            $formstr .= '</div>';
                            $formstr .= '<div class="modal-body">';
                            $formstr .= '<form class="form-horizontal">';
                            $formstr .= '<legend>Please select from the following extras</legend>';
                            $formstr .= '<input type="hidden" id="menuid" name="menuid" value="' . $Menu->ID . '" />';
                            $formstr .= '<input type="hidden" id="storeid" name="storeid" value="' . $Menu->StoreID . '" />';
                            $formstr .= '<input type="hidden" id="locationid" name="locationid" value="' . $Menu->LocationID . '" />';
                            $formstr .= '<input type="hidden" id="itemid" name="itemid" value="' . $itemid . '" />';
                            if ((isset($mpdesc)) && ($mpdesc != ""))
                            {
                                $formstr .= '<input type="hidden" id="mpdesc" name="mpdesc" value="' . $mpdesc . '" />';
                            }
                            $counter = 0;
                            foreach($choicedets as $choice)
                            {
                                $formstr .= '<div class="control-group">';
                                $formstr .= '<label class="radio">';
                                $formstr .= '<input type="radio" name="choicesel" id="choicsel' . $counter . '" value="' . $choice['id'] . '"';
                                if ($mandatory == "Y")
                                {
                                    $formstr .= ' required ';
                                }
                                $formstr .= ' />';
                                $formstr .= $choice['name'] . ' Cost: ' . $choice['cost'];
                                $formstr .= '</label>';
                                $formstr .= '</div>';
                                $counter++;
                            }
                            $formstr .= '</div>';
                            $formstr .= '<div class="modal-footer">';
                            $formstr .= '<button type="submit" formaction="' . SITE_PATH . 'make-choice/" class="btn btn-success">Choose selected</button>';
                            if ($mandatory == "N")
                            {
                            	$formstr .= '<button class="btn btn-danger" data-dismiss="modal" aria-hidden="true">No Thanks</button>';
                            }
                            $formstr .= '</div>';
                            $formstr .= '</form>';
                            $found = true;
                        }
                        break;
                    case "C":
                        $type = 'chfrom';
                        $value = $choiceparts[2];
                        $noofchoices = $resp['noofchoices'];
                        $mandatory = $resp['choice_mandatory'];
                        switch ($choiceparts[1])
                        {
                            case 0:
                                //category
                                $choicedets = $Menu->getItemsbyCat($value);
                                $choicetype = $Menu->getCategorybyId($value);
                                $msg = 'Please choose ' . $noofchoices . ' meal(s) from the ' . $choicetype . ' category to accompany your meal';
                                break;
                            case 1:
                                //meal type
                                $value -= 1;
                                $choicedets = $Menu->getItemsbyMealType($value);
                                $choicetype = $Menu->getMealTypebyID($value);
                                $msg = 'Please choose ' . $noofchoices . ' ' . $choicetype . ' meal(s) from the following to accompany your meal.';
                                break;
                            case 2:
                                //maincat
                                $value -= 1;
                                $choicedets = $Menu->getItemsbyMainCat($value);
                                $choicetype = $Menu->getMainCatbyID($value);
                                $msg = 'Please choose ' . $noofchoices . ' ' . $choicetype . ' to accompany your meal.';
                                break;
                        }

                        if((count($choicedets) >= 1) && ($found != true))
                        {
                            $formstr = '<div class="modal-header">';
                            $formstr .= '<h4 id="choicesextrasModalLabel">Please Choose:</h4>';
                            $formstr .= '</div>';
                            $formstr .= '<div class="modal-body">';
                            $formstr .= '<form class="form-horizontal">';
                            $formstr .= '<legend>' . $msg . '</legend>';
                            $formstr .= '<input type="hidden" id="menuid" name="menuid" value="' . $Menu->ID . '" />';
                            $formstr .= '<input type="hidden" id="storeid" name="storeid" value="' . $Menu->StoreID . '" />';
                            $formstr .= '<input type="hidden" id="locationid" name="locationid" value="' . $Menu->LocationID . '" />';
                            $formstr .= '<input type="hidden" id="itemid" name="itemid" value="' . $itemid . '" />';
                            if ((isset($mpdesc)) && ($mpdesc != ""))
                            {
                                $formstr .= '<input type="hidden" id="mpdesc" name="mpdesc" value="' . $mpdesc . '" />';
                            }
                            $str = "";
                            $str .= '<option value="none">Please Select</option>';
                            foreach($choicedets as $choice)
                            {
                                $str .= '<option value="' . $choice['id'] . '">' . $choice['name'] . '</option>';
                            }
                            for($c=0;$c<=($noofchoices-1);$c++)
                            {
                                $curr = $c+1;
                                $formstr .= '<div class="control-group">';
                                $formstr .= '<h5>Choice ' . $curr . '</h5>';
                                $formstr .= '<select class="choicesel" name="choicesel' . $c . '"';
                                if ($mandatory == "Y")
                                {
                                    $formstr .= ' required="required"';
                                }
                                $formstr .= '>';
                                $formstr .= $str;
                                $formstr .= '</select>';
                                $formstr .= '</div>';
                            }
                            $formstr .= '</div>';
                            $formstr .= '<div class="modal-footer">';
                            $formstr .= '<button type="submit" formaction="' . SITE_PATH . 'make-choice/" class="btn btn-success" id="">Choose selected</button>';
                            if ($mandatory != "Y")
                            {
                                $formstr .= '<button class="btn btn-danger" data-dismiss="modal" aria-hidden="true">No Thanks</button>';
                            }
                            $formstr .= '</div>';
                            $formstr .= '</form>';
                            $found = true;
                        }
                        break;
                }   
            }
        }
    }
    if ($found === true) {
        $ret = array('htmlstr' => $formstr);
    }else{
        $ret = array('htmlstr' => '');
    }

    print_r(json_encode($ret));
    die();
?>
