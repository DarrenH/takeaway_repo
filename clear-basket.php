<?php
	session_start();

	require_once('App.kfc');
    require_once(SERVER_ROOT . 'includes/Class_DatabaseInterface2.inc');
    require_once(SERVER_ROOT . 'includes/Class_Store.inc');
    require_once(SERVER_ROOT . 'includes/Class_Basket.inc');
    require_once(SERVER_ROOT . 'includes/Class_Menu.inc');
    require_once(SERVER_ROOT . 'includes/Class_User.inc');
    require_once(SERVER_ROOT . 'includes/functions.inc');

    $Basket = new Basket();
    if ((isset($_SESSION['basket_id'])) && ($_SESSION['basket_id'] != ''))
    {
        $Basket->setBasketVar('ID',$_SESSION['basket_id']);
    }
    $Basket->clearBasket();
    $html = $Basket->prepareBasketHTML('pop');

    $retarray = array('html' => $html);

    print_r(json_encode($retarray));
    die();
?>