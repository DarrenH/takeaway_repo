<?php
	session_start();

	require_once('App.kfc');
    require_once(SERVER_ROOT . 'includes/functions.inc');
    require_once(SERVER_ROOT . 'includes/Class_DatabaseInterface2.inc');
    require_once(SERVER_ROOT . 'includes/Class_Basket.inc');
    require_once(SERVER_ROOT . 'includes/Class_Menu.inc');
    require_once(SERVER_ROOT . 'includes/Class_User.inc');

    $newUser = checklogged();
    if (isset($newUser))
    {
        $user_id = $newUser->ID;
    }
    else
    {
        $user_id = 0;
    }

    if (isset($_SESSION['basket_id']))
    {
        $Basket = new Basket;
    	$Basket->setBasketVar('ID', $_SESSION['basket_id']);
        if(isset($_GET))
        {
            $storeid = $_GET['storeid'];
            $locationid = $_GET['locationid'];
            $menuid = $_GET['menuid'];
            $itemid = $_GET['itemid'];
            $mpdesc = $_GET['mpdesc'];

            $Menu = new Menu();
            $Menu->setMenuVar("ID", $menuid);
            $Menu->setMenuVar("StoreID", $storeid);
            $Menu->setMenuVar("LocationID", $locationid);

            $icat = $Menu->getItemCategory($itemid);
            $resp = $Menu->getItem($itemid, $icat);

            $noofchoices = $resp['noofchoices'];
            $choices = array();
            for ($i = 0; $i<=$noofchoices-1;$i++)
            {
                $tag = 'cid' . $i;
                array_push($choices, $_GET[$tag]);
            }
            $Basket->saveChoices($storeid, $locationid, $menuid, $itemid, $mpdesc, $choices);

            print_r(json_encode(array(0 => 'true')));
            die();
        }
    }
    print_r(json_encode(array(0 => 'false')));
    die();
?>