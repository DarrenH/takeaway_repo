<?php
	session_start();

	require_once('App.kfc');
    require_once(SERVER_ROOT . 'includes/Class_DatabaseInterface2.inc');
    require_once(SERVER_ROOT . 'includes/Class_Store.inc');
    require_once(SERVER_ROOT . 'includes/Class_Basket.inc');
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

    $ret = 'fail';
    if ((isset($_SESSION['basket_id'])) && ($_SESSION['basket_id'] != ''))
    {
        $Basket = new Basket();

        $Basket->setBasketVar('ID', $_SESSION['basket_id']);
        $Basket->getBasket('login');

        switch ($_GET['step']) {
            case 1:
                //delivery selection and address
                $ret = $Basket->getBasketDeliveryFormHTML();
                break;
            case 2:
                $delcharge = 0.00;
                if (isset($_GET['dtype']) && ($_GET['dtype'] == "D"))
                {
                    $Store = new Store();
                    $Store->getStore($Basket->StoreID);
                    $Store->setStoreVar('SetLocationID', $Basket->LocationID);
                    $Store->getDeliveryDetails2();
                    $location_array = $Basket->geoLocateCustomer($_GET['dhouse'],$_GET['dstreet'],$_GET['dtown'],$_GET['dcounty'],$_GET['dpcode']);
                    $storecoords = array('lat' => $Store->Locations[$Store->SetLocationID]['geolat'], 'long' => $Store->Locations[$Store->SetLocationID]['geolong']);

                    $distance = $Basket->calculateDistanceToStore($location_array,$storecoords);
                    if ($Store->Locations[$Store->SetLocationID]['delivery_type'] === 2)
                    {
                        $pcode = $_GET['dpcode'];
                        unset($cart);
                    }
                    else
                    {
                        unset($pcode,$cart);
                    }

                    $Basket->calculateDeliveryCharge2($distance,$pcode);

                    $deltype = "D";
                    $delinfotype = $Store->Locations[$Store->SetLocationID]['delivery_type'];
                }

                if ($Basket->OODA == 1)
                {
                    //do not deliver here...send back to last step...
                    $addr = array($_GET['dhouse'],$_GET['dstreet'],$_GET['dtown'],$_GET['dcounty'],$_GET['dpcode']);
                    $msg = '<strong>Error</strong>' . $Basket->DeliveryMsg;
                    unset($deltype,$delinfo);
                    $ret = $Basket->getBasketDeliveryFormHTML($msg,$addr);
                }
                else
                {
                    if ($_GET['dtype'] == "D")
                    {
                        $dtype = "D";
                    }
                    else
                    {
                        $dtype = "C";
                    }
                    $Basket->AddUSerInfo($_GET['dhouse'],$_GET['dstreet'],$_GET['dtown'],$_GET['dcounty'],$_GET['dpcode'],$Basket->DeliveryAmount,$dtype);
                    $ret = $Basket->prepareConfirmOrderHTML($Basket->DeliveryAmount,$deltype,$delinfotype,$Basket->DeliveryMsg);
                }
                break;
            case 3:
                //final confirmation
                $ret = $Basket->getBasketContactInfo();
                break;
            case 4:
                //sending
                $ufname = $_GET['ufname'];
                $usname = $_GET['usname'];
                $uemail = $_GET['uemail'];
                $utelno = $_GET['utel'];
                if (isset($_GET['accreate'])) {
                    $accreate = $_GET['accreate'];
                    $pass = encryption($_GET['passval']);
                }
                else
                {
                    unset($accreate,$pass);
                }
                $infarray = array('firstname' => $ufname,'surname' => $usname,'email' => $uemail,'telno' => $utelno, 'account' => $accreate, 'password' => $pass);
                unset($ufname,$usname,$uemail,$utelno);
                $Basket->updateBasketInfo($infarray);
                $accountflag = false;
                if ($accreate == 1)
                {
                    $accountflag = true;
                    $res2 = $Basket->getBasketInfo();
                    $Cust = new Customer();
                    $Cust->createNewCustomer($infarray,$res2);
                    $Basket->updateUserID($Cust->ID);
                    unset($Cust);
                }
                $ret = $Basket->prepareBasketPaymentForm($accountflag);
                break;
            case 5:
                //paying cash or card
                $ret = $Basket->processAsOrder($_GET['type']);
                $Basket->updateStatus('Both');
                break;
            case 6:
                //paypal form return
                $res2 = $Basket->getAllInfo();
                $ret = $Basket->preparePaypalForm($res2);
                break;
            case 7:
                $Basket->clearBasket();
                $ret = 'success';
                break;
        }
    }

    $retarray = array('htmlstr' => $ret);

    print_r(json_encode($retarray));
    die();
?>