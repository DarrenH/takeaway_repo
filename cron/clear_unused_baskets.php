<?php
    require_once('/home/eatinoro/public_html/App.kfc');
    require_once('/home/eatinoro/public_html/includes/Class_DatabaseInterface2.inc');
    require_once('/home/eatinoro/public_html/includes/Class_Basket.inc');

    $cutofftime = time();

    $fields = array(0 => 'id', 1 => 'time');
    $DBA = new DatabaseInteface();
    $rs = $DBA->selectQuery(DBBASKETTABLE, $fields);
    if ((!isset($rs[2])) && ($rs[1] != 0))
    {
        while ($res = mysql_fetch_array($rs[0]))
        {
            //check the difference in times between basket and cutoff. if difference greater than 2 days we can assume it can fuck off.
            $basket_exists_time = gettimestampdifference($cutofftime, $res['time']);
            if ($basket_exists_time['days'] >= 2)
            {
                $tBasket = new Basket();
                $tBasket->setBasketVar('ID', $res['id']);
                $tBasket->deleteBasket();
                unset($tBakset);
            }
        }
    } 
    unset($rs, $res, $fields, $DBA);
    die();
?>