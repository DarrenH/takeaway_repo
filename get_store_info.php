<?php
	session_start();

	require_once('App.kfc');
    require_once(SERVER_ROOT . 'includes/Class_DatabaseInterface2.inc');
    require_once(SERVER_ROOT . 'includes/Class_Store.inc');
    require_once(SERVER_ROOT . 'includes/Class_Menu.inc');
    require_once(SERVER_ROOT . 'includes/functions.inc');

    $Store = new Store();
    $Store->getStore(1);
    $location_id=0;
    $Store->setStoreVar('SetLocationID',0);
    
    $Store->getOLMenu();
    if ($Store->Locations[$location_id]['delivers'] == 'Y')
    {
        $Store->getDeliveryDetails2();
        $Store->prepareDeliveryDetails2();
    }

    $temp = array('store' => get_object_vars($Store));
    print_r(json_encode($temp));
    die();
?>
