<?php
    session_start();

    require_once('App.kfc');
    require_once(SERVER_ROOT . 'includes/functions.inc');
    require_once(SERVER_ROOT . 'includes/Class_DatabaseInterface2.inc');
    require_once(SERVER_ROOT . 'includes/Class_Basket.inc');
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

    if ($_GET['json'] == "true")
    {
        /*javascript call return json*/
        $menuid = $_GET['mid'];
        $itemid = $_GET['iid'];
        if ((isset($_GET['mpdesc'])) && ($_GET['mpdesc'] != ''))
        {
            $multipricedesc = str_replace('_',' ',$_GET['mpdesc']);
        }
    }
    else
    {
        /*$menuid = "";
        $itemid = "";
        if (1=1)
        {
            $multipricedesc = "";
        }*/
    }

    $Basket = new Basket;
    if (isset($_SESSION['basket_id']))
    {
        //existing basket
        $Basket->setBasketVar('ID',$_SESSION['basket_id']);
        $Basket->getBasket('login');
        if ($Basket->checkForExistingItem($itemid,$multipricedesc) === false)
        {
            $Basket->addItem2($menuid,$itemid,$multipricedesc);
        }
        else
        {
            $Basket->updateItem($menuid,$itemid,$multipricedesc);
        }
    }
    else
    {
        $Basket->createNewBasketID();
        $Basket->createBasket($user_id, 1, $menuid, 0, $Basket->ID);
        $Basket->addItem2($menuid,$itemid,$multipricedesc);
    }
    die();
?>