<?php
    session_start();

    require_once('App.kfc');
    require_once(SERVER_ROOT . 'includes/functions.inc');
    require_once(SERVER_ROOT . 'includes/Class_DatabaseInterface2.inc');
    require_once(SERVER_ROOT . 'includes/Class_Menu.inc');
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

    $Basket = new Basket;
    if (isset($_SESSION['basket_id']))
    {
        //existing basket
        $Basket->setBasketVar('ID',$_SESSION['basket_id']);
        $retarray = $Basket->getTotals();
    }
    else
    {
        $retarray = "";
    }
    
    print_r(json_encode($retarray));
    die();
?>