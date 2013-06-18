<?php
    session_start();

    require_once('App.kfc');
    require_once(SERVER_ROOT . 'includes/functions.inc');
    require_once(SERVER_ROOT . 'includes/Class_DatabaseInterface2.inc');
    require_once(SERVER_ROOT . 'includes/Class_Basket.inc');
    require_once(SERVER_ROOT . 'includes/Class_Store.inc');
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

    if ($_GET['json'] == "true")
    {
        /*javascript call return json*/
        $basket_table_id = $_GET['basket_table_id'];
        $type = $_GET['type'];
    }

    $Basket = new Basket;
    if (isset($_SESSION['basket_id']))
    {
        //existing basket
        $Basket->setBasketVar('ID',$_SESSION['basket_id']);
        $Basket->getBasket('login');
        $Basket->basketButtonAction($type, $basket_table_id);
        unset($Basket);
        $Basket = new Basket();
        $Basket->setBasketVar('ID',$_SESSION['basket_id']);
        $Basket->getBasket('login');
        $html = $Basket->prepareBasketHTML('pop');
        $return = array('html' => $html);
        print_r(json_encode($return));
    }
    die();
?>