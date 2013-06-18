<?php
    Class Basket {
        var $ID,
        $UserID,
        $StoreID,
        $LocationID,
        $MenuID,
        $MenuItems,
        $TotalItemCost,
        $Discount,
        $VATAmount,
        $FinalCost,
        $DeliveryType,
        $DeliveryAmount,
        $DeliveryMsg,
        $TotalincludesDelivery,
        $DeliveryChoice,
        $Error,
        $OODA,
        $House,
        $Street,
        $Town,
        $County,
        $PCode,
        $Contel,
        $ContactEmail,
        $FTO,
        $PaymentMethod,
        $PaymentStatus,
        $ExtrasTotal,
        $ChoicesTotal,
        $PPReturned,
        $CustReturned;

        function Basket() {
        }

        function setBasketVar($varname, $varvalue)
        {
            $this->$varname = $varvalue;
        }

        function createBasket($user, $store,$menu,$location)
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'user_id', 1 => 'store_id', 2 => 'menu_id', 3 => 'id', 4 => 'location_id', 5 => 'time');
            $fieldvals = array(0 => $user, 1 => $store, 2 => $menu, 3 => $this->ID, 4 => $location, 5 => time());

            $result = $DBA->insertQuery(DBBASKETTABLE, $fields, $fieldvals);

            if (isset($result))
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        function createNewBasketID() {
            $aZ09 = array_merge(range('A', 'Z'), range('a', 'z'),range(0, 9));
            $ok = 0;
            while ($ok == 0):
                $out ='';
                for($c=0;$c < 20;$c++) {
                    $bid .= $aZ09[mt_rand(0,count($aZ09)-1)];
                }

                $fields = array(0 => 'id');
                $idfields = array(0 => 'user_id', 1 => 'store_id', 2 => 'location_id', 3 => 'id');
                $idvals = array(0 => $user_id, 1 => $store_id, 2 => $location_id, 3 => $bid);

                $DBA = new DatabaseInterface();
                $rs = $DBA->selectQuery(DBBASKETTABLE, $fields, $idfields, $idvals);

                if (isset($rs[2]))
                {
                    $ok = 1;
                }
                elseif($rs[1] == 0)
                {
                    $ok = 1;
                    $basketid = $bid;
                }
                unset($bid);
                unset($fields, $idfields, $idvals, $rs);
            endwhile;
            $_SESSION['basket_id'] = $basketid;
            $this->setBasketVar('ID', $basketid);
        }

        function getTotals()
        {
            $totalcost = 0;
            $totalitems = 0;

            $DBA = new DatabaseInterface();
            $fields = array(0 => 'number_of_items', 1 => 'cost_single', 2 => 'hasextras', 3 => 'haschoices', 4 => 'item_id', 5 => 'multiprice_description',6 => 'id');
            $idfields = array(0 => 'basket_id');
            $idvals = array(0 => $this->ID);
            $rs = $DBA->selectQuery(DBBASKETITEMSTABLE, $fields, $idfields, $idvals);

            if (!isset($rs[2])) {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $totalitems += $res['number_of_items'];
                    $totalcost += $res['number_of_items'] * $res['cost_single'];
                    if ($res['hasextras'] == "Y")
                    {
                        //prepare extras string
                        //$itname .= ' with extras';
                        $extraslist = $this->getExtrasList($res['id']);
                        foreach ($extraslist as $extra)
                        {
                            if ($extra['cost'] > 0)
                            {
                                $totalcost += ($extra['cost'] * $res['number_of_items']);
                            }
                        }
                    }
                    if ($res['haschoices'] == "Y")
                    {
                        //prepare choices string
                        //$itname .= ' with choices';
                        $choicelist = $this->getChoicesList($res['id']);

                        $Menu = new Menu();
                        $Menu->setMenuVar("ID", $this->MenuID);
                        $Menu->setMenuVar("StoreID",$this->StoreID);
                        $Menu->setMenuVar("LocationID", $this->LocationID);

                        $chtype = $Menu->getItemChoiceType($res['item_id']);
                        
                        $choiceparts = explode('/', $chtype);
                        foreach ($choicelist as $choice)
                        {
                            switch ($choiceparts[0]) {
                            case "X":
                                $name = $this->getChoiceInformation($res['item_id'], $choice['choice_id'],$res['multiprice_description']);
                                break;
                            case "C":
                                $name = $Menu->getItemName($choice['choice_id']);
                                break;
                            }
                            //show the choice
                            if ($choice['cost'] >0)
                            {
                                $totalcost += ($choice['cost'] * $res['number_of_items']);
                            }
                        }
                    }
                }
                $totalcost = number_format($totalcost,2);
            }

            unset($DBA, $rs, $res, $fields, $idfields, $idvals);
            return (array('totitems' => $totalitems, 'totalcost' => $totalcost));
        }

        function updateBasketRecord()
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'user_id', 1 => 'store_id', 2 => 'menu_id', 3 => 'location_id');
            $fieldvals = array(0 => $this->UserID, 1 => $this->StoreID, 2 => $this->MenuID, 3 => $this->LocationID);
            $idfield = 'id';
            $idvals = $this->ID;
            $rs = $DBA->updateQuery(DBBASKETTABLE, $fields, $fieldvals, $idfield, $idvals);
        }

        function getBasket($type = null)
        {
            $DBA = new DatabaseInterface();

            $fields = array(0 => 'user_id', 1 => 'store_id', 2 => 'menu_id', 3 => 'location_id', 4 => 'ppreturned', 5 => 'cusreturned');
            $idfields = array(0 => 'id');
            $idvals = array(0 => $this->ID);

            if ($type != 'login')
            {
                array_push($idfields, 'store_id', 'menu_id', 'location_id');
                array_push($idvals, $this->StoreID, $this->MenuID, $this->LocationID);
            }

            $rs = $DBA->selectQuery(DBBASKETTABLE, $fields, $idfields, $idvals);

            if ((!isset($rs[2])) && ($rs[1] >= 1))
            {
                while($res = mysql_fetch_array($rs[0]))
                {
                    $this->setBasketVar('UserID', $res['user_id']);
                    $this->setBasketVar('StoreID', $res['store_id']);
                    $this->setBasketVar('MenuID', $res['menu_id']);
                    $this->setBasketVar('LocationID', $res['location_id']);
                    $this->setBasketVar('PPReturned', $res['ppreturned']);
                    $this->setBasketVar('CustReturned', $res['cusreturned']);
                    $this->setBasketVar('ExtrasTotal', $this->getExtrasTotalCost());
                    $this->setBasketVar('ChoicesTotal', $this->getChoicesTotalCost());
                }
            }
            unset($fields, $idfields, $idvals, $rs, $res, $DBA);
        }

        function checkExistingBasket() {
            $DBA = new DatabaseInterface();

            $fields = array(0 => 'user_id', 1 => 'store_id', 2 => 'menu_id', 3 => 'location_id');
            $idfields = array(0 => 'id');
            $idvals = array(0 => $this->ID);

            $rs = $DBA->selectQuery(DBBASKETTABLE, $fields, $idfields, $idvals);

            if ((!isset($rs[2])) && ($rs[1] >= 1))
            {}
            else
            {
                //basket record isnt there but basket id session is set
                $this->createBasket($_SESSION['user_id'], $_SESSION['store_id'],$_SESSION['menu_id'],$_SESSION['location_ID']);
            }
            unset($DBA, $rs, $fields, $idfields, $idvals);
        }

        function getBasketItems()
        {
            $menuitems = array();

            $DBA = new DatabaseInterface();
            $fields = array(0 => 'item_id', 1 => 'number_of_items', 2 => 'cost_single', 3 => 'multiprice_description', 4 => 'hasextras', 5 => 'id', 6 => 'haschoices');
            $idfields = array(0 => 'basket_id');
            $idvals = array(0 => $this->ID);
            $orderby = 'item_id, multiprice_description, id';

            $rs = $DBA->selectQuery(DBBASKETITEMSTABLE, $fields, $idfields, $idvals, $orderby);
            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    array_push($menuitems, $res);
                }
            }

            $this->setBasketVar('MenuItems', $menuitems);

            unset($fields, $idfields, $idvals, $rs, $res, $DBA, $menuitems);

            foreach ($this->MenuItems as $item)
            {
                $itemcost = number_format($item['cost_single'] * $item['number_of_items'], 2);
                $totalcost += $itemcost;
            }

            $this->setBasketVar('TotalItemCost', number_format($totalcost ,2));
            //$discount = checkDiscount();
            $vat = ($this->TotalItemCost / 100) * VAT_RATE;
            $this->setBasketVar('Discount', 0);
            $this->setBasketVar('VATAmount', $vat);
            $this->setBasketVar('FinalCost', number_format($this->TotalItemCost - $discount, 2));
        }

        function UpdateBasket()
        {
            //Reworks totals and updates records
            foreach ($this->MenuItems as $item)
            {
                $itemcost = $item['cost_single'] * $item['number_of_items'];
                $totalcost += $itemcost;
            }

            $this->setBasketVar('TotalItemCost', number_format($totalcost,2));
            //$discount = checkDiscount();
            $discount = 0;
            $this->setBasketVar('Discount', 0);
            $vat = ($this->TotalItemCost / 100) * VAT_RATE;
            $this->setBasketVar('VATAmount', $vat);
            $this->setBasketVar('FinalCost', number_format($this->TotalItemCost - $discount, 2));
        }

        function RemoveItem($itemid, $qty = null, $type = null, $mp = null, $basket_table_id = null)
        {
            //removes item completely from basket unless qty is set in which case remove that qty
            if (count($this->MenuItems >= 1))
            {
                foreach($this->MenuItems as $item)
                {
                    if ($item['item_id'] == $itemid)
                    {
                        if ((isset($qty)) && ($qty !=""))
                        {
                            if ((($mp != "") && ($mp == $item['multiprice_description'])) || ($mp == ""))
                            {
                                if ((!isset($basket_table_id)) || ($basket_table_id == "") || ((isset($basket_table_id)) && ($item['id'] == $basket_table_id)))
                                {
                                    //lower amount in basket table
                                    $DBA = new DatabaseInterface();

                                    $fields = array(0 => 'id', 1 => 'number_of_items');
                                    $idfields = array(0 =>'item_id', 1 => 'basket_id');
                                    $idvals = array(0 => $itemid, 1 => $this->ID);
                                    if ($mp != "")
                                    {
                                        array_push($idfields, 'multiprice_description');
                                        array_push($idvals, $mp);
                                    }

                                    if ((isset($basket_table_id)) && ($basket_table_id != ""))
                                    {
                                        array_push($idfields, 'id');
                                        array_push($idvals, $basket_table_id);
                                    }

                                    $rs = $DBA->selectQuery(DBBASKETITEMSTABLE, $fields, $idfields, $idvals);

                                    if (!isset($rs[2]))
                                    {
                                        while ($res = mysql_fetch_array($rs[0]))
                                        {
                                            $existid = $res['id'];
                                            $num = $res['number_of_items'];
                                        }
                                    }

                                    unset($fields, $idfields, $idvals, $rs, $res, $DBA);

                                    $newnum = $num - $qty;

                                    $DBA = new DatabaseInterface();

                                    if ($newnum !=0)
                                    {
                                        $fields = array(0 => 'number_of_items');
                                        $fieldvals = array(0 => $newnum);
                                        $idfields = array(0 => 'id');
                                        $idvals = array(0 => $existid);
                                        if ($mp != "")
                                        {
                                            array_push($idfields, 'multiprice_description');
                                            array_push($idvals, $mp);
                                        }
                                        $rs = $DBA->updateQuery(DBBASKETITEMSTABLE, $fields, $fieldvals, $idfields, $idvals);
                                        if (!isset($rs))
                                        {
                                            if (!isset($type))
                                            {
                                                error_push('Error updating quantity');
                                            }
                                        }
                                        else
                                        {
                                            if (!isset($type))
                                            {
                                                session_push('Quantity updated');
                                            }
                                        }
                                        unset($fields, $idfields, $idvals, $fieldvals, $rs, $DBA, $newnum, $num, $existid);
                                    }
                                    else
                                    {
                                        $DBA = new DatabaseInterface();
                                        $fields = array(0 => '*');
                                        $idfields = array(0 => 'basket_id', 1 => 'item_id');
                                        $idvals = array(0 => $this->ID, 1 => $itemid);
                                        if ($mp != "")
                                        {
                                            array_push($idfields, 'multiprice_description');
                                            array_push($idvals, $mp);
                                        }

                                        if ((isset($basket_table_id)) && ($basket_table_id != ""))
                                        {
                                            array_push($idfields, 'id');
                                            array_push($idvals, $basket_table_id);
                                        }

                                        $rs = $DBA->selectQuery(DBBASKETITEMSTABLE, $fields, $idfields, $idvals);

                                        if (!isset($rs[2]))
                                        {
                                            while ($res = mysql_fetch_array($rs[0]))
                                            {
                                                $btid = $res['id'];
                                                $hasextras = $res['hasextras'];
                                                $haschoices = $res['haschoices'];
                                            }
                                        }
                                        unset($fields, $idfields, $idvals, $rs, $res, $DBA);

                                        $DBA = new DatabaseInterface();
                                        $sql = "delete from " . DBBASKETITEMSTABLE . " where basket_id = '" . $this->ID . "' and item_id = " . $itemid;
                                        if ($mp != "")
                                        {
                                            $sql .= " and multiprice_description = '" . $mp . "'";
                                        }

                                        if ((isset($basket_table_id)) && ($basket_table_id != ""))
                                        {
                                            $sql .= " and id = '" . $basket_table_id . "'";
                                        }

                                        $rs = $DBA->runQuery($sql);

                                        if (!isset($rs))
                                        {
                                            if (!isset($type))
                                            {
                                                error_push('Error removing item');
                                            }
                                        }
                                        else
                                        {
                                            if (!isset($type))
                                            {
                                                session_push('Item removed');
                                            }
                                        }
                                        unset($DBA, $sql, $rs);

                                        if ($hasextras == 'Y')
                                        {
                                            $DBA = new DatabaseInterface();
                                            $sql = "delete from " . DBBASKETEXTRAITEMSTABLE . " where basket_id='" . $this->ID . "' and item_id = " . $itemid;
                                            if (isset($basket_table_id))
                                            {
                                                $sql .= " and basket_table_id = '" . $basket_table_id . "'";
                                            }
                                            $rs = $DBA->runQuery($sql);
                                        }

                                        if ($haschoices == 'Y')
                                        {
                                            $DBA = new DatabaseInterface();
                                            $sql = "delete from " . DBBASKETCHOICEITEMSTABLE . " where basket_id='" . $this->ID . "' and item_id = " . $itemid;
                                            if (isset($basket_table_id))
                                            {
                                                $sql .= " and basket_table_id = '" . $basket_table_id . "'";
                                            }
                                            $rs = $DBA->runQuery($sql);
                                        }
                                        unset($DBA, $sql, $rs);
                                    }
                                    $this->updateBasket();
                                }
                            }
                        }
                        else
                        {
                            //remove record from basket table
                            if ((($mp != "") && ($mp == $item['multiprice_description'])) || ($mp == "") || (!isset($mp)))
                            {
                                $DBA = new DatabaseInterface();
                                $fields = array(0 => '*');
                                $idfields = array(0 => 'basket_id', 1 => 'item_id');
                                $idvals = array(0 => $this->ID, 1 => $itemid);
                                if ($mp != "")
                                {
                                    array_push($idfields, 'multiprice_description');
                                    array_push($idvals, $mp);
                                }

                                if ((isset($basket_table_id)) && ($basket_table_id !=""))
                                {
                                    array_push($idfields, 'id');
                                    array_push($idvals, $basket_table_id);
                                }

                                $rs = $DBA->selectQuery(DBBASKETITEMSTABLE, $fields, $idfields, $idvals);

                                if (!isset($rs[2]))
                                {
                                    while ($res = mysql_fetch_array($rs[0]))
                                    {
                                        $btid = $res['id'];
                                        $hasextras = $res['hasextras'];
                                        $haschoices = $res['haschoices'];
                                    }
                                }

                                unset($fields, $idfields, $idvals, $rs, $res, $DBA);
                                $DBA = new DatabaseInterface();
                                $sql = "delete from " . DBBASKETITEMSTABLE . " where basket_id = '" . $this->ID . "' and item_id = " . $itemid;
                                if ($mp != "")
                                {
                                    $sql .= " and multiprice_description = '" . $mp . "'";
                                }

                                if ((isset($basket_table_id)) && ($basket_table_id != ""))
                                {
                                    $sql .= " and id = '" . $basket_table_id . "'";
                                }

                                $rs = $DBA->runQuery($sql);
                                if (!isset($rs))
                                {
                                    if (!isset($type))
                                    {
                                        error_push('Error removing item');
                                    }
                                }
                                else
                                {
                                    if (!isset($type))
                                    {
                                        session_push('Item removed');
                                    }
                                }
                                unset($DBA, $sql, $rs);
                                if ($hasextras == 'Y')
                                {
                                    $DBA = new DatabaseInterface();
                                    $sql = "delete from " . DBBASKETEXTRAITEMSTABLE . " where basket_id='" . $this->ID . "' and item_id = " . $itemid;
                                    if ((isset($basket_table_id)) && ($basket_table_id != ""))
                                    {
                                        $sql .= " and basket_table_id = '" . $basket_table_id . "'";
                                    }
                                    $rs = $DBA->runQuery($sql);
                                }
                                unset($DBA, $sql, $rs);
                            
                                if ($haschoices == "Y")
                                {
                                    $DBA = new DatabaseInterface();
                                    $sql = "delete from " . DBBASKETCHOICEITEMSTABLE . " where basket_id='" . $this->ID . "' and item_id = " . $itemid;
                                    if (isset($basket_table_id))
                                    {
                                        $sql .= " and basket_table_id = '" . $basket_table_id . "'";
                                    }
                                    $rs = $DBA->runQuery($sql);
                                }
                                unset($DBA, $sql, $rs);
                            }
                        }
                    }
                }
            }
        }

        function checkForExistingItem($iid,$mpdescr = null) {
            $res = false;
            $DBA = new DatabaseInterface();
            $fields = array(0 => '*');
            $idfields = array(0 => 'basket_id', 1 => 'item_id');
            $idvals = array(0 => $this->ID, 1 => $iid);
            if ((isset($mpdescr)) && (!is_null($mpdescr)))
            {
                array_push($idfields, 'multiprice_description');
                array_push($idvals, $mpdescr);
            }
            $rs = $DBA->selectQuery(DBBASKETITEMSTABLE, $fields, $idfields, $idvals);

            if (!isset($rs[2]))
            {
                if ($rs[1] >= 1)
                {
                    $res = true;
                }
            }

            unset($DBA, $rs, $fields, $idfields, $idvals);

            return $res;
        }

        function AddItem2($menuid, $itemid, $mp)
        {
            //we have already checked for existing this is a new item!
            $icost = $this->getItemPrice($menuid,$itemid,$mp);
            $qty = 1;
            $DBA = new DatabaseInterface();
            $id = getmaxid(DBBASKETITEMSTABLE);
            $fields = array(0 => 'id', 1 => 'basket_id', 2 => 'item_id', 3 => 'number_of_items', 4 => 'cost_single');
            $fieldvals = array(0 => $id, 1 => $this->ID, 2 => $itemid, 3 => $qty, 4 => $icost);
            if ($mp != "")
            {
                array_push($fields, 'multiprice_description');
                array_push($fieldvals, $mp);
            }

            $rs = $DBA->insertQuery(DBBASKETITEMSTABLE, $fields, $fieldvals);
            return true;
        }

        function updateItem($menuid, $itemid, $mp) {
            //first off we check if the existing item in the basket has extras...if it does then we need to make a new record for it
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'hasextras', 1 => 'haschoices', 2 => 'id', 3 => 'number_of_items');
            $idfields = array(0 => 'basket_id', 1 => 'item_id');
            $idvals = array(0 => $this->ID, 1 => $itemid);
            if ((isset($mpdesc)) && (!is_null($mp)))
            {
                array_push($idfields, 'multiprice_description');
                array_push($idvals, $mpdesc);
            }
            $rs = $DBA->selectQuery(DBBASKETITEMSTABLE, $fields, $idfields, $idvals);

            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    if (($res['hasextras'] == "Y") || ($res['haschoices'] == "Y"))
                    {
                        $this->AddItem2($menuid, $itemid, $mp);
                    }
                    else
                    {
                        //otherwise just increment the number_of_items by one and send back
                        $this->incrementTotalItem($res['id'], $res['number_of_items']);
                    }
                }
            }
            else
            {
                $this->AddItem2($menuid, $itemid, $mp);
            }

            return (true);
        }

        function incrementTotalItem($basketitemid, $noi) {
            $noi += 1;
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'number_of_items');
            $fieldvals = array(0 => $noi);
            $idfields = 'id';
            $idvals = $basketitemid;

            $rs = $DBA->updateQuery(DBBASKETITEMSTABLE, $fields, $fieldvals, $idfields, $idvals);

            return true;
        }

        function getItemPrice($menuid, $itemid, $multiprice = null)
        {

            $DBA = new DatabaseInterface();
            $fields = array(0 => 'cost', 1 => 'ptype');
            $idfields = array(0 =>'id', 1 => 'menu_id');
            $idvals = array(0 => $itemid, 1 => $menuid);

            $rs = $DBA->selectQuery(DBOLMENUITEMSTABLE, $fields, $idfields, $idvals);

            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    if ($res['ptype'] == 'S')
                    {
                        $cost = $res['cost'];
                    }
                    else
                    {
                        //multiprice
                        $DBA2 = new DatabaseInterface();
                        $fields2 = array(0 => 'cost');
                        $idfields2 = array(0 => 'menu_item_id', 1 => 'description');
                        $idvals2 = array(0 => $itemid, 1 => str_replace('_', ' ',$multiprice));

                        $rs2 = $DBA2->selectQuery(DBITEMMULTIPLEPRICETABLE, $fields2, $idfields2, $idvals2);

                        if (!isset($rs2[2]))
                        {
                            while ($res2 = mysql_fetch_array($rs2[0]))
                            {
                                $cost = $res2['cost'];
                            }
                        }
                        unset($fields2, $idfields2, $idvals2, $rs2, $res2, $DBA2);
                    }
                }
            }

            unset($fields, $idfields, $idvals, $rs, $res, $DBA);

            return($cost);
        }

        function AddItem($menuid, $itemid, $qty, $type = null, $mp = null, $btid = null, $reuse = null)
        {
            unset($updated);
            if ($qty == 0)
            {
                $qty = 1;
            }
            //inserts the given item id into the list of ids within the basket
            //search for itemid existing and add new qty
            //else create new list item with id and stuff
            unset($tupdate_id, $tqty);
            $match = false;

            if (count($this->MenuItems) >= 1)
            {
                foreach($this->MenuItems as $item)
                {
                    if ($item['item_id'] == $itemid)
                    {
                        if (($mp == "") || (($item['multiprice_description'] != '') && ($item['multiprice_description'] == $mp)))
                        {
                            if (($item['hasextras'] != "Y") && ($item['haschoices'] != "Y"))
                            {
                                if ((!isset($btid)) || ($btid == ""))
                                {
                                    $update_id = $item['id'];
                                    $newqty = $item['number_of_items'] += $qty;
                                    $updated = 1;
                                    $match = true;
                                    break;
                                }
                            }
                            else
                            {
                                if ((!is_null($btid)) && ($item['id'] == $btid))
                                {
                                    $tupdate_id = $item['id'];
                                    $tnewqty = $item['number_of_items'] += $qty;
                                }
                            }
                        }
                    }
                }

                if (($match == false) && (isset($tupdate_id)))
                {
                    $update_id = $tupdate_id;
                    $newqty = $tnewqty;
                    $updated = 1;
                }
            }
            
            if (!isset($updated))
            {
                $DBA = new DatabaseInterface();
                $fields = array(0 => 'cost', 1 => 'ptype');
                $idfields = array(0 =>'id', 1 => 'menu_id');
                $idvals = array(0 => $itemid, 1 => $menuid);

                $rs = $DBA->selectQuery(DBOLMENUITEMSTABLE, $fields, $idfields, $idvals);

                if (!isset($rs[2]))
                {
                    while ($res = mysql_fetch_array($rs[0]))
                    {
                        if ($res['ptype'] == 'S')
                        {
                            $cost = $res['cost'];
                        }
                        else
                        {
                            //multiprice
                            $DBA2 = new DatabaseInterface();
                            $fields2 = array(0 => 'cost');
                            $idfields2 = array(0 => 'menu_item_id', 1 => 'description');
                            $idvals2 = array(0 => $itemid, 1 => str_replace('_', ' ',$mp));

                            $rs2 = $DBA2->selectQuery(DBITEMMULTIPLEPRICETABLE, $fields2, $idfields2, $idvals2);

                            if (!isset($rs2[2]))
                            {
                                while ($res2 = mysql_fetch_array($rs2[0]))
                                {
                                    $cost = $res2['cost'];
                                }
                            }
                            unset($fields2, $idfields2, $idvals2, $rs2, $res2, $DBA2);
                        }
                    }
                }

                unset($fields, $idfields, $idvals, $rs, $res, $DBA);

                if ($mp != "")
                {
                    $newitem = array('item_id' => $itemid, 'number_of_items' => $qty, 'cost_single' => $cost, 'mp_desc' => $mp);
                }
                else
                {
                    $newitem = array('item_id' => $itemid, 'number_of_items' => $qty, 'cost_single' => $cost);
                }
                array_push($this->MenuItems, $newitem);

                $DBA = new DatabaseInterface();
                $id = getmaxid(DBBASKETITEMSTABLE);
                $fields = array(0 => 'id', 1 => 'basket_id', 2 => 'item_id', 3 => 'number_of_items', 4 => 'cost_single');
                $fieldvals = array(0 => $id, 1 => $this->ID, 2 => $itemid, 3 => $qty, 4 => $cost);
                if ($mp != "")
                {
                    array_push($fields, 'multiprice_description');
                    array_push($fieldvals, $mp);
                }

                $rs = $DBA->insertQuery(DBBASKETITEMSTABLE, $fields, $fieldvals);

                if (!isset($rs))
                {
                    if (!isset($type))
                    {
                        error_push('Error adding item');
                    }
                }
                else
                {
                    if (!isset($type))
                    {
                        session_push('Item added');
                    }
                }
                $this->updateBasket();
            }
            else
            {
                if (!isset($update_id))
                {
                    $DBA = new DatabaseInterface();

                    $fields = array(0 => 'id', 1 => 'number_of_items');
                    $idfields = array(0 =>'item_id', 1 => 'basket_id');
                    $idvals = array(0 => $itemid, 1 => $this->ID);
                    if ($mp != "")
                    {
                        array_push($idfields, 'multiprice_description');
                        array_push($idvals, $mp);
                    }
                    if (isset($btid))
                    {
                        array_push($idfields, 'id');
                        array_push($idvals, $btid);
                    }

                    $rs = $DBA->selectQuery(DBBASKETITEMSTABLE, $fields, $idfields, $idvals);
                    if (!isset($rs[2]))
                    {
                        while ($res = mysql_fetch_array($rs[0]))
                        {
                            $existid = $res['id'];
                            $num = $res['number_of_items'];
                        }
                    }

                    unset($fields, $idfields, $idvals, $rs, $res, $DBA);

                    $newnum = $num + $qty;
                }
                else
                {
                    $existid = $update_id;
                    $newnum = $newqty;
                }

                $DBA = new DatabaseInterface();
                $fields = array(0 => 'number_of_items');
                $fieldvals = array(0 => $newnum);
                $idfields = array(0 => 'id');
                $idvals = array(0 => $existid);
                if ($mp != "")
                {
                    array_push($idfields, 'multiprice_description');
                    array_push($idvals, $mp);
                }

                $rs = $DBA->updateQuery(DBBASKETITEMSTABLE, $fields, $fieldvals, $idfields, $idvals);
                if (!isset($rs))
                {
                    if (!isset($type))
                    {
                        error_push('Error updating quantity');
                    }
                }
                else
                {
                    if (!isset($type))
                    {
                        session_push('Quantity updated');
                    }
                }
                unset($fields, $idfields, $idvals, $fieldvals, $rs, $DBA, $newnum, $num, $existid);
                $this->updateBasket();
            }
        }

        function convertToOrder($orderref, $printer_message)
        {
            //create header
            $id = getmaxid(DBORDERHEADERTABLE);
            $fields = array(0 => 'user_id', 1 => 'store_id', 2 => 'location_id', 3 => 'menu_id', 4 => 'discount', 5 => 'deliverytype', 6 => 'deliverycost', 7 => 'id', 8 => 'reference', 9 => 'time', 10 => 'printer_message', 11 => 'contactemail', 12 => 'paymenttype', 13 => 'payment_status');
            $fieldvals = array(0 => $this->UserID, 1 => $this->StoreID, 2 => $this->LocationID, 3 => $this->MenuID, 4 => $this->Discount, 5 => $this->DeliveryChoice, 6 => $this->DeliveryAmount, 7 => $id, 8 => $orderref, 9 => time(), 10 => $printer_message, 11 => $this->ContactEmail, 12 => $this->PaymentType, 13 => $this->PaymentStatus);
            if ($this->DeliveryChoice == 'D')
            {
                array_push($fields, 'house', 'street', 'town', 'county', 'pcode', 'contacttelephone');
                array_push($fieldvals, $this->House, $this->Street, $this->Town, $this->County, $this->PCode, $this->Contel);
            }
            $DBA = new DatabaseInterface();
            $rs = $DBA->insertQuery(DBORDERHEADERTABLE, $fields, $fieldvals);

            if ($rs == 1)
            {
                //create items
                foreach ($this->MenuItems as $item)
                {
                    $DBA2 = new DatabaseInterface();
                    $fields2 = array(0 => 'name');
                    $idfields2 = array(0 => 'id', 1 => 'menu_id');
                    $idvals2 = array(0 => $item['item_id'], 1 => $this->MenuID);
                    $rs2 = $DBA2->selectQuery(DBOLMENUITEMSTABLE, $fields2, $idfields2, $idvals2);
                    if (!isset($rs[2]))
                    {
                        while ($res2 = mysql_fetch_array($rs2[0]))
                        {
                            $name = $res2['name'];
                        }
                    }
                    unset($DBA2, $rs2, $res2, $fields2, $idfields2, $idvals2);

                    //copy $items to order items table
                    $iid = getmaxid(DBORDERITEMSTABLE);
                    $fields = array(0 => 'id', 1 => 'order_id', 2 => 'item_id', 3 => 'number_of_items', 4 => 'cost_single', 5 => 'name', 6 => 'multiprice_description', 7 => 'hasextras', 8 => 'haschoices');
                    $fieldvals = array(0 => $iid, 1 => $id, 2 => $item['item_id'], 3 => $item['number_of_items'], 4 => $item['cost_single'], 5 => $name, 6 => $item['multiprice_description'], 7 => $item['hasextras'], 8 => $item['haschoices']);

                    $DBA2 = new DatabaseInterface();
                    $rs2 = $DBA2->insertQuery(DBORDERITEMSTABLE, $fields, $fieldvals);

                    if (!$rs2 == 1)
                    {
                        $this->setBasketVar('Error', 'Error creating item');
                    }

                    unset($fields, $fieldvals, $DBA2, $rs2);

                    if ($item['hasextras'] == "Y")
                    {
                        //copy extras for this item into order items extra table
                        $extraslist = $this->getExtrasList($item['id']);
                        if (count($extraslist) >= 1)
                        {
                            foreach($extraslist as $extra)
                            {
                                $eid = getmaxid(DBORDERITEMEXTRASTABLE);
                                $extraname = $this->getExtraName($item['item_id'], $extra['extra_id']);
                                $fields3 = array(0 => 'id', 1 => 'order_id', 2 => 'order_table_id', 3 => 'item_id', 4 => 'extra_id', 5 => 'cost', 6 => 'name');
                                $fieldvals3 = array(0 => $eid, 1 => $id, 2 => $iid, 3 => $item['item_id'], 4 => $extra['extra_id'], 5 => $extra['cost'], 6 => $extraname);
                                $DBA3 = new DatabaseInterface();
                                $rs3 = $DBA3->insertQuery(DBORDERITEMEXTRASTABLE, $fields3, $fieldvals3);
                                if (!$rs3 == 1)
                                {
                                    $this->setBasketVar('Error', 'Error adding extra ' . $extra['name']);
                                }
                                unset($rs3, $fieldvals3, $fields3, $DBA3, $eid);
                            }
                        }
                    }
                    if ($item['haschoices'] == "Y")
                    {
                        $choiceslist = $this->getChoicesList($item['id']);
                        if (count($choiceslist) >= 1)
                        {
                            $choicetype = $this->getItemChoiceType($item['item_id']);
                            $choiceparts = explode('/', $choicetype);
                            foreach($choiceslist as $choice)
                            {
                                $cid = getmaxid(DBORDERITEMCHOICESTABLE);
                                if($choiceparts[0] == 'X')
                                {
                                    $choiceinfo = $this->getChoiceInformation($item['item_id'], $choice['choice_id']);
                                    $name = $choiceinfo['name'];
                                    $cost = number_format($choiceinfo['cost'], 2);
                                }
                                else
                                {
                                    $cost = number_format(0,2);
                                    $name = $this->getItemName($choice['choice_id']);
                                }
                                $fields3 = array(0 => 'id', 1 => 'order_id', 2 => 'order_table_id', 3 => 'item_id', 4 => 'choice_id', 5 => 'cost', 6 => 'name');
                                $fieldvals3 = array(0 => $cid, 1 => $id, 2 => $iid, 3 => $item['item_id'], 4 => $choice['choice_id'], 5 => $choice['cost'], 6 => $name);
                                $DBA3 = new DatabaseInterface();
                                $rs3 = $DBA3->insertQuery(DBORDERITEMCHOICESTABLE, $fields3, $fieldvals3);
                                if (!$rs3 == 1)
                                {
                                    $this->setBasketVar('Error', 'Error adding extra choices.');
                                }
                                unset($rs3, $fieldvals3, $fields3, $DBA3, $eid);
                            }
                        }
                    }
                }
                $this->DeleteBasket();
            }
            else
            {
                $this->setBasketVar('Error', "Error saving header record");

            }
        }
        
        function getItemChoiceType($iid)
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'choice_type');
            $idfields = array(0 => 'menu_id', 1 => 'id');
            $idvals = array(0 => $this->MenuID, 1 => $iid);

            $rs = $DBA->selectQuery(DBOLMENUITEMSTABLE, $fields, $idfields, $idvals);

            if ((!isset($rs[2])) && ($rs[1] >= 1))
            {
                while($res = mysql_fetch_array($rs[0]))
                {
                    $choice_type = $res['choice_type'];
                }
            }

            unset($fields, $idfields, $idvals, $rs, $res, $DBA);

            return($choice_type);
        }

        function deleteBasket()
        {
            $idfield = 'basket_id';
            $idval = $this->ID;
            $DBA = new DatabaseInterface();
            $rs = $DBA->deleteRecord(DBBASKETITEMSTABLE, $idfield, $idval);

            if (!isset($rs[2]))
            {
                $idfield = 'id';
                $DBA2 = new DatabaseInterface();
                $rs = $DBA2->deleteRecord(DBBASKETTABLE, $idfield, $idval);
                if (!isset($rs[2]))
                {
                    $idfield = "basket_id";
                    $idval = $this->ID;
                    $DBA3 = new DatabaseInterface();
                    $rs3 = $DBA->deleteRecord(DBBASKETEXTRAITEMSTABLE, $idfield, $idval);
                }
            }
            $_SESSION['basketid'] = '';
            unset($DBA, $DBA2, $DBA3);
        }

        function clearBasket() {
            $idfields = array(0 => 'basket_id');
            $idvals = array(0 => $this->ID);

            $DBA = new DatabaseInterface();

            $rs = $DBA->deleteRecord(DBBASKETITEMSTABLE,$idfields,$idvals);
            $rs = $DBA->deleteRecord(DBBASKETEXTRAITEMSTABLE,$idfields,$idvals);
            $rs = $DBA->deleteRecord(DBBASKETCHOICEITEMSTABLE,$idfields,$idvals);
            $rs = $DBA->deleteRecord(DBBASKETINFOTABLE,$idfields,$idvals);

            $idfields = array(0 => 'id');
            $rs = $DBA->deleteRecord(DBBASKETTABLE,$idfields,$idvals);

            unset($rs,$idfields,$idvals,$DBA);

            unset($_SESSION['basket_id']);
        }

        /*function calculateDeliveryCharge($distance=0)
        {
            global $Store;
            //sort store deliverycharges
            switch ($Store->Locations[$Store->SetLocationID]['delivery_type']) {
                case 1:
                    //free above $widgStore->FreeDeliveryAmount
                    $deliveryamount = 0;
                    if ($this->TotalItemCost < $Store->DeliveryAreas[0]['free_delivery_amount'])
                    {
                        $deliveryamount = $Store->DeliveryAreas[0]['cost'];
                        $msg = '<p>* As your order does not exceed the amount to get free delivery a surcharge has been added to your total. If you are collecting your order then this amount will be removed at checkout. Free delivery is available at an order value of ' . $Store->DeliveryAreas[0]['free_delivery_amount'] . '.</p>';
                    }
                    elseif ($this->TotalItemCost > $Store->DeliveryArea[0]['free_delivery_amount'])
                    {
                        $msg = '<p>* Your order qualifies for free delivery. You do not need to do anything to claim this your total has been updated automatically.</p>';
                    }

                    if ($distance > $Store->DeliveryAreas[0]['max_radius'])
                    {
                        $this->OODA = true;
                        $msg .= '<p>The address associated with your account is also outside of the delivery area of this takeaway. You will need to enter a new address within the delivery area if you wish to have your order delivered.</p>';
                    }
                    break;
                case 2:
                    $deliveryamount = 0;
                    break;
                case 3:
                    $deliveryamount = 0;
                    break;
                default:
                    $deliveryamount = 0;
            }
            $this->setBasketVar('DeliveryAmount', number_format($deliveryamount, 2));
            $this->setBasketVar('DeliveryType', $Store->Locations[$Store->SetLocationID]['delivery_type']);
            $this->setBasketVar('DeliveryMsg', $msg);
        }*/

        function calculateDeliveryCharge2($distance, $pcode = null, $cart = null)
        {
            global $Store;

            $Store2 = new Store();
            $Store2->getStore($this->StoreID);

            if (($distance > $Store->DeliveryInformation['max_radius']) && ($Store->Locations[$Store->SetLocationID]['delivery_type'] != 2))
            {
                $this->OODA = true;
                $this->setBasketVar('DeliveryMsg', '<p>Sorry but you are outside the delivery area for this address. You will have to collect your order.</p>');
                $this->setBasketVar('DeliveryAmount', 0);
            }
            else
            {
                switch ($Store->Locations[$Store->SetLocationID]['delivery_type']) {
                    case 1:
                        //multiple types
                        $choicetot = $Store->DeliveryInformation['choices'][0] . $Store->DeliveryInformation['choices'][1];
                        $itemscost = $this->TotalItemCost;
                        $itemscost += $this->ExtrasTotal;
                        $itemscost += $this->ChoicesTotal;
                        $itemscost += number_format($itemscost, 2);
                        switch ($choicetot) {
                            case 11:
                                //delivery set at set rate for all
                                $this->setBasketVar('DeliveryAmount', number_format($Store->DeliveryInformation['variables'][0]['var2'],2));
                                $this->setBasketVar('DeliveryMsg', '<p>* This store does not offer free delivery only charged at the cost shown.</p>');
                                break;
                            case 12:
                                //charged at x per mile
                                unset($found);
                                foreach ($Store->DeliveryInformation['variables'] as $varset)
                                {
                                    if (($distance <= $varset['var1']) && (!$found))
                                    {
                                        //match
                                        $this->setBasketVar('DeliveryAmount', number_format($varset['var2'], 2));
                                        $this->setBasketVar('DeliveryMsg', '<p>* The amount of delivery charged depends on the distance from the store. See the delivery details section for more information.</p>');
                                        $found = true;
                                        break;
                                    }
                                }
                                break;
                            case 13:
                                //no delivery
                                $this->setBasketVar('DeliveryAmount', 'No delivery');
                                $this->setBasketVar('DeliveryMsg', '<p>* This store does not deliver, you will have to collect your order.</p>');
                                break;
                            case 21:
                                if ($itemscost >= $Store->DeliveryInformation['variables'][0]['var1'])
                                {
                                    $this->setBasketVar('DeliveryAmount', number_format(0,2));
                                    $this->setBasketVar('DeliveryMsg', '<p>* Your order qualifies for free delivery. You do not need to do anything to claim this your total has been updated automatically.</p>');
                                }
                                else
                                {
                                    $this->setBasketVar('DeliveryAmount', number_format($Store->DeliveryInformation['variables'][0]['var2'],2));
                                    $this->setBasketVar('DeliveryMsg', '<p>* As your order does not exceed the amount to get free delivery there has been a surcharge placed on the order. All orders that do not qualify for free delivery are charged at a standard rate. Free delivery is available at an order value of &pound;' . number_format($Store->DeliveryInformation['variables'][0]['var1'], 2) . '.</p>');
                                }
                                break;
                            case 22:
                                if ($itemscost >= $Store->DeliveryInformation['variables'][0]['var1'])
                                {
                                    $this->setBasketVar('DeliveryAmount', number_format(0,2));
                                    $this->setBasketVar('DeliveryMsg', '<p>* Your order qualifies for free delivery. You do not need to do anything to claim this your total has been updated automatically.</p>');
                                }
                                else
                                {
                                    unset($found);
                                    foreach ($Store->DeliveryInformation['variables'] as $varset)
                                    {
                                        if (($distance <= $varset['var2']) && (!$found))
                                        {
                                            //match
                                            $this->setBasketVar('DeliveryAmount', number_format($varset['var3'], 2));
                                            $this->setBasketVar('DeliveryMsg', '<p>* The amount of delivery charged depends on the distance from the store. See the delivery details section for more information. Free delivery is available at an order value of &pound;' . number_format($Store->DeliveryInformation['variables'][0]['var1'],2) . '.</p>');
                                            $found = true;
                                        }
                                    }
                                }
                                break;
                            case 23:
                                //free delivery over &pound;x no delivery under that
                                if ($itemscost >= $Store->DeliveryInformation['variables'][0]['var1'])
                                {
                                    $this->setBasketVar('DeliveryAmount', number_format(0,2));
                                    $this->setBasketVar('DeliveryMsg', '<p>* Your order qualifies for free delivery. You do not need to do anything to claim this your total has been updated automatically.</p>');
                                }
                                else
                                {
                                    $this->OODA = true;
                                    $this->setBasketVar('DeliveryAmount', "No delivery");
                                    $this->setBasketVar('DeliveryMsg', '<p>* As your order does not exceed the amount to get free delivery this takeaway will not deliver and you will have to collect your order. Free delivery is available at an order value of &pound;' . number_format($Store->DeliveryInformation['variables'][0]['var1'],2) . '.</p>');
                                }
                                break;
                            case 31:
                                if ($distance <= $Store->DeliveryInformation['variables'][0]['var1'])
                                {
                                    //inside free
                                    $this->setBasketVar('DeliveryAmount', number_format(0,2));
                                    $this->setBasketVar('DeliveryMsg', '<p>* Your order qualifies for free delivery. You do not need to do anything to claim this your total has been updated automatically.</p>');
                                }
                                else
                                {
                                    $this->setBasketVar('DeliveryAmount', number_format($Store->DeliveryInformation['variables'][0]['var2']));
                                    $this->setBasketVar('DeliveryMsg', '<p>* Your order does not qualify for free delivery as you do not live within ' . $Store->DeliveryInformation['variables'][0]['var1'] . ' miles of the store. All orders over ' . $Store->DeliveryInformation['variables'][0]['var1'] . ' miles are charged at the cost shown.  Free delivery is only available to residences inside ' . $Store->DeliveryInformation['variables'][0]['var1'] . ' miles of the store.</p>');
                                }
                                break;
                            case 32:
                                if ($distance <= $Store->DeliveryInformation['variables'][0]['var1'])
                                {
                                    //inside free
                                    $this->setBasketVar('DeliveryAmount', number_format(0,2));
                                    $this->setBasketVar('DeliveryMsg', '<p>* Your order qualifies for free delivery. You do not need to do anything to claim this your total has been updated automatically.</p>');
                                }
                                else
                                {
                                    unset($found);
                                    foreach ($Store->DeliveryInformation['variables'] as $varset)
                                    {
                                        if (($distance <= $varset['var2']) && (!$found))
                                        {
                                            //match
                                            $this->setBasketVar('DeliveryAmount', number_format($varset['var3'], 2));
                                            $this->setBasketVar('DeliveryMsg', '<p>* The amount of delivery charged depends on the distance from the store. See the delivery details section on this stores page to check. Free delivery is only available to residences inside ' . $Store->DeliveryInformation['variables'][0]['var1'] . ' miles of the store.</p>');
                                            $found = true;
                                        }
                                    }
                                }
                                break;
                            case 33:
                                if ($distance <= $Store->DeliveryInformation['variables'][0]['var1'])
                                {
                                    $this->setBasketVar('DeliveryAmount', number_format(0,2));
                                    $this->setBasketVar('DeliveryMsg', '<p>* Your order qualifies for free delivery. You do not need to do anything to claim this your total has been updated automatically.</p>');
                                }
                                else
                                {
                                    $this->OODA = true;
                                    $this->setBasketVar('DeliveryAmount', "No delivery");
                                    $this->setBasketVar('DeliveryMsg', '<p>* As your order does not exceed the amount to get free delivery this takeaway will not deliver and you will have to collect your order. Free delivery is only available to residences inside ' . $Store->DeliveryInformation['variables'][0]['var1'] . ' miles.</p>');
                                }
                                break;
                        }
                        break;
                    case 2:
                        //postcode
                        if ((!isset($pcode)) || (is_postcode($pcode) === false))
                        {
                            //message should say not determingable
                            if (is_postcode($pcode) === false)
                            {
                                //bad postcode
                                if ($cart != 1)
                                {
                                    $this->setBasketVar('DeliveryMsg', '<p>This store only offers delivery to certain postcodes. Please enter a valid postcode below and click to process the order. You will have a chance to review the new delivery charge, if any, before payment is made.</p>');
                                }
                                else
                                {
                                    $this->setBasketVar('DeliveryMsg', '<p>This store only offers delivery to certain postcodes. Please enter a valid postcode when you place your order. You will have a chance to review the new delivery charge, if any, before payment is made.</p>');
                                }
                            }
                            else
                            {
                                if ($distance == 999)
                                {
                                    if($cart != 1)
                                    {
                                        //not signed in so register method
                                        $this->setBasketVar('DeliveryMsg', '<p>* As you have not registered for an account at ' . SITE_NAME . ' we cannot determine your location.</p><p>You could <a href="' . REGISTER_LINK . '" title="register for a free account">register for a free account</a> and enter your preferred delivery address to see it calculated in your basket (Just one of the many extras and benefits of being a member) otherwise enter your delivery address below and click to process the order. You will have a chance to review the new delivery charge, if any, before payment is made.</p><p>You can check out ' . $Store->Name . ' delivery prices on the <a href="#delivery" title="Delivery">delivery tab</a>.</p><p>If you already have account please login, don\'t worry you won\'t lose anything you have put in your basket.</p>');
                                    }
                                    else
                                    {
                                        $this->setBasketVar('DeliveryMsg', '<p>* As you have not registered for an account at ' . SITE_NAME . ' we cannot determine your location.</p><p>You could <a href="' . REGISTER_LINK . '" title="register for a free account">register for a free account</a> and enter your preferred delivery address to see it calculated in your basket (Just one of the many extras and benefits of being a member) otherwise enter your delivery address when you place your order. You will have a chance to review the new delivery charge, if any, before payment is made.</p><p>You can check out ' . $Store->Name . ' delivery prices on the <a href="#delivery" title="Delivery">delivery tab</a>.</p><p>If you already have account please login, don\'t worry you won\'t lose anything you have put in your basket.</p>');
                                    }
                                }
                                else
                                {
                                    //signed in but no postcode set!
                                    if ($cart != 1)
                                    {
                                        $this->setBacketVar('DeliveryMsg', '<p>* This store only offers delivery to certain postcodes and you do not have a postcode listed under you user information! Please <a href="' . ACCOUNT_LINK . ' title="My Filing Cabinet">update your user information</a> or enter the delivery address below and click to process the order. You will have a chance to review the new delivery charge, if any, before payment is made.</p>');
                                    }
                                    else
                                    {
                                        $this->setBacketVar('DeliveryMsg', '<p>* This store only offers delivery to certain postcodes and you do not have a postcode listed under you user information! Please <a href="' . ACCOUNT_LINK . ' title="My Filing Cabinet">update your user information</a> or enter the delivery address when you place your order. You will have a chance to review the new delivery charge, if any, before payment is made.</p>');
                                    }
                                }
                            }
                            $this->setBasketVar('DeliveryAmount', number_format(0,2));
                            $this->setBasketVar('OODA', 1);
                        }
                        elseif (($Store->Locations[$Store->SetLocationID]['delivery_type'] == 2) && ($itemstotal < $Store->Locations[$Store->SetLocationID]['min_order_value']))
                        {
                            //not over minimum order value
                            $this->OODA = 1;
                            $this->setBasketVar('DeliveryMsg', '<p>* This store only offers delivery to certain postcodes and only on orders over &pound;' . number_format($Store->Locations[$Store->SetLocationID]['min_ord_value'],2) . '. To qualify for delivery you will need to add some more items to your order.</p>');
                        }
                        elseif (count($Store->DeliveryInformation['variables']) >= 1)
                        {
                            unset($pcodematch, $pcodecost);
                            $match = false;
                            $this->setBasketVar('DeliveryAmount', number_format(0,2));
                            $dicounter = 0;
                            foreach($Store->DeliveryInformation['variables'] as $pcodeinfo)
                            {
                                //$pos = strpos(strtolower($pcode), strtolower($pcodeinfo['var1']));
                                //$pos2 = strpos(strtolower($pcodeinfo['var1']), strtolower($pcode));
                                $pos = stristr($pcodeinfo['var1'], $pcode);
                                $pos2 = stristr($pcode, $pcodeinfo['var1']);
                                if (($pos !== false) || ($pos2 !== false))
                                {
                                    //we have a match
                                    $match = true;
                                    $pcodematch = $Store->DeliveryInformation['variables'][$dicounter]['var1'];
                                    $pcodecost = $Store->DeliveryInformation['variables'][$dicounter]['var2'];
                                }
                                $dicounter++;
                            }
                            if ($match === true)
                            {
                                $this->setBasketVar('DeliveryAmount', number_format($pcodecost,2));
                                if ($cart != 1)
                                {
                                    $this->setBasketVar('DeliveryMsg', '<p>* The amount of delivery charged depends on the postcode you enter for delivery. You can still change your information below. See the delivery details section for more information.</p>');
                                }
                                else
                                {
                                    $this->setBasketVar('DeliveryMsg', '<p>* The amount of delivery charged depends on the postcode you enter for delivery. You can still change your information when you place your order. See the delivery details section for more information.</p>');
                                }
                            }
                            else
                            {
                                //they do not deliver here!
                                $this->setBasketVar('DeliveryMsg', '<p>* This store only offers delivery to certain postcodes and unfortunately the address you entered does not fall into one of those postcodes. Please contact the store to place your order and see if they will be nice and make an exception.</p>');
                                $this->setBasketVar('OODA', 1);
                            }
                        }
                        else
                        {
                            //we have no delivery information despite having been set
                            $this->setBasketVar('DeliveryMsg', '<p>* There was a problem accessing this stores delivery information. Please refresh your screen and try again or contact the store in question.</p>');
                            $this->setBasketVar('OODA', 1);
                        }
                        break;
                }
            }     
        } 
        
        function updatePost($postarray)
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'pppost');
            $fieldvals = array(0 => json_encode($_POST));
            $idfield = 'id';
            $idvals = $this->ID;
            $rs = $DBA->updateQuery(DBBASKETTABLE, $fields, $fieldvals, $idfield, $idvals);
        }  

        function getExtrasList($btid)
        {
            $returnarray = array();

            $DBA = new DatabaseInterface();
            $fields = array(0 => '*');
            $idfields = array(0 => 'basket_id', 1 => 'basket_table_id');
            $idvals = array(0 => $this->ID, 1 => $btid);

            $rs = $DBA->selectQuery(DBBASKETEXTRAITEMSTABLE, $fields, $idfields, $idvals);

            if ((!isset($rs[2])) && ($rs[1] >= 1))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    array_push($returnarray, $res);
                }
            }
            unset($rs, $res, $DBA, $fields, $idfields, $idvals);
            return ($returnarray);
        }

        function getChoicesList($btid)
        {
            $returnarray = array();

            $DBA = new DatabaseInterface();
            $fields = array(0 => '*');
            $idfields = array(0 => 'basket_id', 1 => 'basket_table_id');
            $idvals = array(0 => $this->ID, 1 => $btid);

            $rs = $DBA->selectQuery(DBBASKETCHOICEITEMSTABLE, $fields, $idfields, $idvals);

            if ((!isset($rs[2])) && ($rs[1] >= 1))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    array_push($returnarray, $res);
                }
            }
            unset($rs, $res, $DBA, $fields, $idfields, $idvals);
            return ($returnarray);
        }

        function getChoiceInformation($iid, $chid,$mpdesc=null)
        {
            $DBA3 = new DatabaseInterface();
            $fields3 = array(0 => '*');
            $idfields3 = array(0 => 'item_id', 1 => 'menu_id', 2 => 'id', 3 => 'store_id', 4 => 'location_id');
            $idvals3 = array(0 => $iid, 1 => $this->MenuID, 2 => $chid, 3 => $this->StoreID, 4 => $this->LocationID);

            if (isset($mpdesc))
            {
                array_push($idfields3, 'mpdescription');
                array_push($idvals3, $mpdesc);
            }

            $rs3 = $DBA3->selectQuery(DBMENUITEMCHOICETABLE, $fields3, $idfields3, $idvals3);

            if ((!isset($rs3[2])) && ($rs3[1] >= 1))
            {
                while ($res3 = mysql_fetch_array($rs3[0]))
                {
                    $retname = $res3;
                }
            }

            unset($DBA3, $fields3, $idfields3, $idvals3, $rs3);

            return($retname);    
        }

        function getItemName($iid)
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'name');
            $idfields = array(0 => 'id', 1 => 'menu_id');
            $idvals = array(0 => $iid, 1 => $this->MenuID);
            $rs = $DBA->selectQuery(DBOLMENUITEMSTABLE, $fields, $idfields, $idvals);
            if ((!isset($rs[2])) && ($rs[1] >= 1))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $retname = $res['name'];
                }
            }

            unset($DBA, $fields, $idfields, $idvals, $rs, $res);

            return $retname;
        }

        function checkFirstTimeOrder()
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => "count(id) as total");
            $idfields = array(0 => 'user_id', 1 => 'store_id', 2 => 'location_id');
            $idvals = array(0 => $this->UserID, 1 => $this->StoreID, 2 => $this->LocationID);
            $rs = $DBA->selectQuery(DBORDERHEADERTABLE, $fields, $idfields, $idvals);
            
            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $tot = $res['total'];
                }
            }
            unset($DBA, $rs, $res, $fields, $idfields);

            if ($tot > 0)
            {
                $this->setBasketVar('FTO', 'false');
            }
            else
            {
                $this->setBasketVar('FTO', 'true');
            }
        }

        function getExtraName($item_id, $extra_id)
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'name');
            $idfields = array(0 => 'store_id', 1 => 'location_id', 2 => 'menu_id', 3 => 'item_id', 4 => 'id');
            $idvals = array(0 => $this->StoreID, 1 => $this->LocationID, 2 => $this->MenuID, 3 => $item_id, 4 => $extra_id);
            $rs = $DBA->selectQuery(DBMENUITEMEXTRASTABLE, $fields, $idfields, $idvals);

            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $ename = $res['name'];
                }
            }

            unset($DBA, $rs, $fields, $idfields, $idvals, $res);

            return ($ename);
        }

        function getExtrasTotalCost()
        {
            $fields = array(0 => 'sum(cost) as totcost');
            $idfields = array(0 => 'basket_id');
            $idvals = array(0 => $this->ID);

            $DBA = new DatabaseInterface();
            $rs = $DBA->selectQuery(DBBASKETEXTRAITEMSTABLE, $fields, $idfields, $idvals);#

            if(!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $tot = $res['totcost'];
                }
            }
            unset($DBA, $rs, $res, $fields, $idfields, $idvals);

            return($tot);
        }

        function getChoicesTotalCost()
        {
            $fields = array(0 => 'sum(cost) as totcost');
            $idfields = array(0 => 'basket_id');
            $idvals = array(0 => $this->ID);

            $DBA = new DatabaseInterface();
            $rs = $DBA->selectQuery(DBBASKETCHOICEITEMSTABLE, $fields, $idfields, $idvals);#

            if(!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $tot = $res['totcost'];
                }
            }
            unset($DBA, $rs, $res, $fields, $idfields, $idvals);

            return($tot);
        }

        function prepareBasketHTML($type=null)
        {
            if ($this->MenuItems == '')
            {
                $this->getBasketItems();
            }
            $str = '';
            $str = '<div class="modal-header">';
            $str .= '<button class="close" data-dismiss="modal" aria-hidden="true">Continue Shopping &times;</button>';
            $str .= '<h4 id="viewBasketLabel">Basket</h4>';
            $str .= '</div>';
            $str .= '<div class="modal-body">';
            if (count($this->MenuItems) != 0)
            {
                $totals = $this->getTotals();

                $Store = new Store();
                $Store->getStore($this->StoreID);
                $Store->setStoreVar('SetLocationID', $this->LocationID);
                $Store->getDeliveryDetails2();
                
                if ($totals['totalcost'] < $Store->DeliveryInformation['min_order_value'])
                {
                    $str .= '<div class="alert alert-block"><h5>Under Mininum Order Value</h5><p>You are under the minimum value for delivery. If you contiue you will have to collect.</p></div>';
                }
                $str .= '<table class="table table-bordered table-striped">';
                $str .= '<thead>';
                $str .= '<tr><th>Item</th><th>Qty</th><th>Cost(per)</th><th>Edit</th></tr>';
                $str .= '</thead>';
                $str .= '<tbody>';
                foreach ($this->MenuItems as $item)
                {
                    $str .= '<tr>';
                    $str .= '<td>' . $this->getItemName($item['item_id']);
                    if ($item['multiprice_description'] != '')
                    {
                        $str .= ' - ' . $item['multiprice_description'];
                    }
                    if ($item['hasextras'] == "Y")
                    {
                        //prepare extras string
                        $extraslist = $this->getExtrasList($item['id']);
                        $str .= '<div class="extraslist"><p>With ';
                        foreach ($extraslist as $extra)
                        {

                            $name = $this->getExtraName($item['item_id'],$extra['extra_id']);
                            $str .= $name;
                            if ($extra['cost'] > 0)
                            {
                                $str .= '&pound;' . number_format($extra['cost'],2);
                            }
                        }
                        $str .= '</p></div>';
                    }
                    if ($item['haschoices'] == "Y")
                    {
                        //prepare choices string
                        $choicelist = $this->getChoicesList($item['id']);
                        $str .= '<div class="choicelist">';
                        $Menu = new Menu();
                        $Menu->setMenuVar("ID", $this->MenuID);
                        $Menu->setMenuVar("StoreID",$this->StoreID);
                        $Menu->setMenuVar("LocationID", $this->LocationID);

                        $chtype = $Menu->getItemChoiceType($item['item_id']);
                        
                        $choiceparts = explode('/', $chtype);
                        $ccounter = 0;
                        if ($item['multiprice_description']!='')
                        {
                            $mp = str_replace(' ','_',$item['multiprice_description']);
                        }
                        else
                        {
                            unset($mp);
                        }
                        foreach ($choicelist as $choice)
                        {
                            switch ($choiceparts[0]) {
                            case "X":
                                $name = $this->getChoiceInformation($item['item_id'], $choice['choice_id'],$mp);
                                $name = $name['name'];
                                break;
                            case "C":
                                $name = $Menu->getItemName($choice['choice_id']);
                                break;
                            }
                            //show the choice
                            $showcount = $ccounter+1;
                            $str .= "Choice " . $showcount . " - " . $name;
                            if ($choice['cost'] >0)
                            {
                                $str .= "&pound;" . number_format($choice['cost'],2);
                            }
                            $str .= '<br />';
                            $ccounter +=1;
                        }
                        $str .= '</div>';
                    }
                    $str .= '</td>';
                    $str .= '<td>' . $item['number_of_items'] . '</td>';
                    $str .= '<td>&pound;' . $item['cost_single'] . '</td>';
                    $str .= '<td>';
                    $str .= '<div class="btn-group">';
                    $str .= '<button class="btn btn-small bskbut add" data-loading-text="...." id="addone' . $item['id'] . '" formaction="../add-one/' . $item['id'] . '"><i class="icon-plus-sign"></i></button>';
                    $str .= '<button class="btn btn-small bskbut rem" data-loading-text="...." id="remone' . $item['id'] . '" formaction="../remove-one/' . $item['id'] . '"><i class="icon-minus-sign"></i></button>';
                    $str .= '<button class="btn btn-small bskbut del" data-loading-text="...." id="del' . $item['id'] . '" formaction="../delete-item/' . $item['id'] . '"><i class="icon-remove-sign"></i></button>';
                    $str .= '</div>';
                    $str .= '</td>';
                    $str .= '</tr>';
                }
                $str .= '</tbody>';
                $str .= '</table>';
                $str .= '<div id="basket-totals">';
                $str .= '<p>Total Items: ' . $totals['totitems'] . '</p>';
                $str .= '<p>Total Cost: &pound;' . number_format($totals['totalcost'],2) . 'p</p>';
                $str .= '</div>';
            }
            else
            {
                $str .= '<div class="alert alert-block"><h5>No Items</h5><p>Please add some items to your basket by using the corresponding "add" button.</p></div>';
            }
            $str .= '</div>';
            $str .= '<div class="modal-footer">';
            $str .= '<button class="btn" data-dismiss="modal" aria-hidden="true" data-loading-text="...">Continue Shopping &times;</button>';
            if (count($this->MenuItems) >= 1)
            {
                $str .= '<button id="clearbsk" data-loading-text="..." class="btn btn-warning" title="Clear Basket">Clear Basket</button>';
                $str .= '<button id="conf" type="submit" data-loading-text="..." class="btn btn-success" title="Confirm Order">Confirm Order</button>';
            }
            $str .= '</div>';

            return ($str);
        }

        function removeAllItems() {
            $this->getBasket();
            $this->getBasketItems('login');
            foreach ($this->MenuItems as $item)
            {
                $this->removeItem($item['item_id'], $item['number_of_items'], 'clear', $item['multiprice_description'], $item['id']);
            }
        }

        function saveExtras($sid, $lid, $mid, $iid, $mpdesc, $exid)
        {
            //we need tp get the basket id of the original item
            $sql = "SELECT id from " . DBBASKETITEMSTABLE . " where basket_id = '" .$this->ID . "' and item_id = '" . $iid . "'";
            $sql .= " and id not in (select basket_table_id from " . DBBASKETEXTRAITEMSTABLE . " where basket_id = '" . $this->ID . "')";
            $DBA = new DatabaseInterface();
            $rs = $DBA->runQuery($sql);

            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    //$res['basket_table_id'] is what we need
                    $btid = $res['id'];
                }
            }

            unset($sql, $DBA, $res, $rs);

            //need to get extras cost from the menu extras table
            $fields = array(0 => 'cost');
            $idfields = array(0 => 'id', 1=> 'store_id', 2 => 'location_id', 3 => 'menu_id');
            $idvals = array(0 => $exid, 1 => $sid, 2 => $lid, 3 => $mid);

            if ((isset($mpdesc)) && ($mpdesc!= ''))
            {
                array_push($idfields, 'mpdescription');
                array_push($idvals, $mpdesc);
            }

            $DBA = new DatabaseInterface();
            $rs = $DBA->selectQuery(DBMENUITEMEXTRASTABLE, $fields, $idfields, $idvals);

            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $excost = $res['cost'];
                }
            }

            unset($DBA,$rs,$res,$fields,$idfields,$idvals);

            $eid = getmaxid(DBBASKETEXTRAITEMSTABLE);
            //now add to the extras table
            $fields = array(0 => 'id', 1 => 'basket_id', 2 => 'basket_table_id', 3 => 'item_id', 4 => 'extra_id', 5 => 'cost');
            $fieldvals = array(0 => $eid, 1 => $this->ID, 2 => $btid, 3 => $iid, 4 => $exid, 5 => $excost);

            $DBA = new DatabaseInterface();
            $rs = $DBA->insertQuery(DBBASKETEXTRAITEMSTABLE, $fields, $fieldvals);

            unset($DBA, $rs, $res, $fields, $fieldvals);

            $fields = array(0 => 'hasextras');
            $fieldvals = array(0 => "Y");
            $idfields = "id";
            $idvals = $btid;

            $DBA = new DatabaseInterface();

            $rs = $DBA->updateQuery(DBBASKETITEMSTABLE,$fields, $fieldvals, $idfields, $idvals);

            unset($DBA, $rs, $fields, $fieldvals, $idfields, $idvals);
        }

        function saveChoices($sid, $lid, $mid, $iid, $mpdesc, $choicelist)
        {
            $sql = "SELECT id from " . DBBASKETITEMSTABLE . " where basket_id = '" .$this->ID . "' and item_id = '" . $iid . "'";
            $sql .= " and id not in (select basket_table_id from " . DBBASKETCHOICEITEMSTABLE . " where basket_id = '" . $this->ID . "')";
            $DBA = new DatabaseInterface();
            $rs = $DBA->runQuery($sql);

            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $btid = $res['id'];
                }
            }

            unset($sql, $DBA, $res, $rs);

            $DBA = new DatabaseInterface();
            $fields = array(0 => 'choice_type');
            $idfields = array(0 => 'menu_id', 1 => 'id');
            $idvals = array(0 => $mid, 1 => $iid);
            $rs = $DBA->selectQuery(DBOLMENUITEMSTABLE, $fields, $idfields, $idvals);

            if ((!isset($rs[2])) && ($rs[1] >= 1))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $chtype = $res['choice_type'];
                }
            }

            $choiceparts = explode('/', $chtype);

            switch ($choiceparts[0]) {
                case "X":
                    //xor
                    $DBA = new DatabaseInterface();
                    $fields = array(0 => 'cost');
                    $idfields = array(0 => 'store_id', 1 => 'location_id', 2 => 'menu_id', 3 => 'item_id', 4 => 'id',5 => 'mpdescription');
                    $idvals = array(0 => $sid, 1 => $lid, 2 => $mid, 3 => $iid, 4 => $choicelist[0],5 => $mpdesc);
                    
                    $rs = $DBA->selectQuery(DBMENUITEMCHOICETABLE, $fields, $idfields, $idvals);
                    
                    if ((!isset($rs[2])) && ($rs[1] >= 1))
                    {
                        while ($res = mysql_fetch_array($rs[0]))
                        {
                            $cost = $res['cost'];
                        }
                    }
                    break;
                case "C":
                    //compulsary
                    $cost = 0;
                    break;
            }

            foreach ($choicelist as $choiceid)
            {
                $ctid = getmaxid(DBBASKETCHOICEITEMSTABLE);
                //now add to the choices table
                $fields = array(0 => 'id', 1 => 'basket_id', 2 => 'basket_table_id', 3 => 'item_id', 4 => 'choice_id', 5 => 'cost');
                $fieldvals = array(0 => $ctid, 1 => $this->ID, 2 => $btid, 3 => $iid, 4 => $choiceid, 5 => $cost);

                $DBA = new DatabaseInterface();
                $rs = $DBA->insertQuery(DBBASKETCHOICEITEMSTABLE, $fields, $fieldvals);

                unset($DBA, $rs, $res, $fields, $fieldvals);
            }

            $fields = array(0 => 'haschoices');
            $fieldvals = array(0 => "Y");
            $idfields = "id";
            $idvals = $btid;

            $DBA = new DatabaseInterface();

            $rs = $DBA->updateQuery(DBBASKETITEMSTABLE,$fields, $fieldvals, $idfields, $idvals);

            unset($DBA, $rs, $fields, $fieldvals, $idfields, $idvals);            
        }

        function basketButtonAction($type,$id) {
            $delflag = false;
            //get noi first
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'number_of_items');
            $idfields = array(0 => 'id');
            $idvals = array(0 => $id);

            $rs = $DBA->selectQuery(DBBASKETITEMSTABLE, $fields,$idfields,$idvals);

            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $num = $res['number_of_items'];
                }
            }
            unset($DBA, $rs, $res);

            switch ($type) {
                case "add":
                    $newnum = $num + 1;
                    break;
                case "rem":

                    $newnum = $num - 1;

                    if ($newnum == 0)
                    {
                        $delflag = true;
                    }
                    break;
                case "del":
                    $idfields = 'id';
                    $idvals = $id;
                    break;
            }

            if (($type == 'del') || ($delflag === true))
            {
                $DBA = new DatabaseInterface();
                $rs = $DBA->deleteRecord(DBBASKETITEMSTABLE,$idfields, $idvals);

                $idfields = array(0 => 'basket_table_id');
                $DBA->deleteRecord(DBBASKETEXTRAITEMSTABLE,$idfields,$idvals);
                $DBA->deleteRecord(DBBASKETCHOICEITEMSTABLE,$idfields,$idvals);
            }
            elseif (($type === 'add') || ($type === 'rem'))
            {
                $fields = array(0 => 'number_of_items');
                $fieldvals = array(0 => $newnum);
                $idfields = array(0 => 'id');
                $idvals = array(0 => $id);

                $DBA = new DatabaseInterface();
                $rs = $DBA->updateQuery(DBBASKETITEMSTABLE, $fields, $fieldvals, $idfields, $idvals);
            }

        }

        function getBasketInfo() {
            unset($retarray);
            $fields = array(0 => 'house', 1 => 'street', 2 => 'town', 3 => 'county', 4 => 'pcode',5 => 'delamount',6 => 'deltype');
            $idfields = array(0 => 'basket_id');
            $idvals = array(0 => $this->ID);

            $DBA = new DatabaseInterface();
            $rs = $DBA->selectQuery(DBBASKETINFOTABLE, $fields, $idfields, $idvals);
            if ((!isset($rs[2])) && ($rs[1] != 0))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $retarray = $res;
                }
            }

            return $retarray;
        }

        function getBasketDeliveryFormHTML($errormessage=null,$getvals=null) {
            if (count($getvals) == 0)
            {
                $getvals = $this->getBasketInfo();
            }
            $Store = new Store();
            $Store->getStore($this->StoreID);
            $Store->setStoreVar('SetLocationID', $this->LocationID);
            $Store->getDeliveryDetails2();
            $str = '';
            $str = '<div class="modal-header">';
            $str .= '<button class="close" data-dismiss="modal" aria-hidden="true">Cancel &times;</button>';
            $str .= '<h4 id="nextLabel">Delivery</h4>';
            $str .= '</div>';
            $str .= '<div class="modal-body">';
            $str .= '<form name="deliveryinfo" id="deliveryinfo" class="form-horizontal">';
            if ($Store->Locations[$Store->SetLocationID]['delivers'] == 'Y')
            {
                $str .= '<h5>Please select whether you want home delivery</h5>';
                if (isset($errormessage))
                {
                    $str .='<div class="alert alert-block alert-error">' . $errormessage . '</div>';
                }
                $str .= '<div class="row-fluid">';
                $tots = $this->getTotals();
                if ($tots['totalcost'] >= $Store->DeliveryInformation['min_order_value']) 
                {
                $str .= '<div class="control-group span4 offset1">';
                $str .= '<label class="delivery"><input required="required" type="radio" name="delval" id="delvalD" value="D" ';
                if(isset($getvals)) { $str .= ' checked';}
                $str .= ' /> Please deliver</label>';
                $str .= '</div>';
                }
                else
                {
                    $str .= '<div class="control-group span4 offset1">';
                    $str .= '<div class="alert">You have not reached the minimum order value for delivery. Please go back and add more items to your basket. Your order total must exceed &pound;' . $Store->DeliveryInformation['min_order_value'] . '</div>';
                    $str .= '</div>';
                }
                $str .= '<div class="control-group span4 offset1">';
                $str .= '<label class="delivery"><input required="required" type="radio" name="delval" id="delvalC" value="C" /> I will collect</label>';
                $str .= '</div>';
                $str .= '</div>';
                if ($tots['totalcost'] >= $Store->DeliveryInformation['min_order_value']) 
                {
                $str .= '<div id="deladdr">';
                $str .= '<div class="control-group">';
                $str .= '<label class="control-label" for="ohouse">House name/number <small>*</small></label>';
                $str .= '<div class="controls"><input required="required" type="text" name="ohouse" id="ohouse" value="' . $getvals[0] . '" /></div>';
                $str .= '</div>';
                $str .= '<div class="control-group">';
                $str .= '<label class="control-label" for="ostreet">Street Name <small>*</small></label>';
                $str .= '<div class="controls"><input required="required" type="text" name="ostreet" id="ostreet" value="' . $getvals[1] . '" /></div>';
                $str .= '</div>';
                $str .= '<div class="control-group">';
                $str .= '<label class="control-label" for="otown">Town</label>';
                $str .= '<div class="controls"><input type="text" name="otown" id="otown" value="' . $getvals[2] . '" /></div>';
                $str .= '</div>';
                $str .= '<div class="control-group">';
                $str .= '<label class="control-label" for="ocounty">County</label>';
                $str .= '<div class="controls"><input type="text" name="ocounty" id="ocounty" value="' . $getvals[3] . '" /></div>';
                $str .= '</div>';
                $str .= '<div class="control-group">';
                $str .= '<label class="control-label" for="opcode">Postcode <small>*</small></label>';
                $str .= '<div class="controls"><input required="required" type="text" name="opcode" id="opcode" value="' . $getvals[4] . '" /></div>';
                $str .= '</div>';
                $str .= '</div>';
            }
            }
            else
            {
                //collection is mandatory so screw any kindof choice just tell them.
                //bonus is that delivery fee is nothing and we have no need to grab an address.
                $str .= '<h5>Collection Only</h5>';
                $str .= '<p>This store does not offer delivery of any kind at present.</p>';
            }
            $str .= '</div>';
            $str .= '<div class="modal-footer">';
            $str .= '<button class="btn" data-dismiss="modal" aria-hidden="true" data-loading-text="...">Cancel &times;</button>';
            $str .= '<button type="submit" id="delprog" data-loading-text="..." class="btn btn-success" title="Confirm Order">Continue</button>';
            $str .= '</div>';
            $str .= '</form>';

            return($str);
        }

        function prepareConfirmOrderHTML($deliveryam = null,$deliverytype=null,$delinfotype=null,$delmsg=null) {
            if ($this->MenuItems == '')
            {
                $this->getBasketItems();
            }
            if (isset($deliveryam))
            {
                $this->updateBasketInfo(array('delam' => $deliveryam,'deltype' => $deliverytype));
            }
            $totals = $this->getTotals();
            $problem = false;
            $str = '';
            $str = '<div class="modal-header">';
            $str .= '<button class="close" data-dismiss="modal" aria-hidden="true">Cancel &times;</button>';
            $str .= '<h4>Confirm</h4>';
            $str .= '</div>';
            $str .= '<div class="modal-body">';
            if (count($this->MenuItems) != 0)
            {
                $str .= '<table class="table table-bordered table-striped">';
                $str .= '<thead>';
                $str .= '<tr><th>Item</th><th>Qty</th><th>Cost(per)</th></tr>';
                $str .= '</thead>';
                $str .= '<tbody>';
                foreach ($this->MenuItems as $item)
                {
                    $str .= '<tr>';
                    $str .= '<td>' . $this->getItemName($item['item_id']);
                    if ($item['multiprice_description'] != '')
                    {
                        $str .= ' - ' . $item['multiprice_description'];
                    }
                    if ($item['hasextras'] == "Y")
                    {
                        //prepare extras string
                        $extraslist = $this->getExtrasList($item['id']);
                        $str .= '<div class="extraslist"><p>With ';
                        foreach ($extraslist as $extra)
                        {

                            $name = $this->getExtraName($item['item_id'],$extra['extra_id']);
                            $str .= $name;
                            if ($extra['cost'] > 0)
                            {
                                $str .= '&pound;' . number_format($extra['cost'],2);
                            }
                        }
                        $str .= '</p></div>';
                    }
                    if ($item['haschoices'] == "Y")
                    {
                        //prepare choices string
                        $choicelist = $this->getChoicesList($item['id']);
                        $str .= '<div class="choicelist">';
                        $Menu = new Menu();
                        $Menu->setMenuVar("ID", $this->MenuID);
                        $Menu->setMenuVar("StoreID",$this->StoreID);
                        $Menu->setMenuVar("LocationID", $this->LocationID);
                        $chtype = $Menu->getItemChoiceType($item['item_id']);
                        
                        $choiceparts = explode('/', $chtype);
                        $ccounter = 0;
                        if ($item['multiprice_description']!='')
                        {
                            $mp = str_replace(' ','_',$item['multiprice_description']);
                        }
                        else
                        {
                            unset($mp);
                        }
                        foreach ($choicelist as $choice)
                        {
                            switch ($choiceparts[0]) {
                            case "X":
                                $ret = $this->getChoiceInformation($item['item_id'], $choice['choice_id'],$mp);
                                $name = $ret['name'];
                                break;
                            case "C":
                                $name = $Menu->getItemName($choice['choice_id']);
                                break;
                            }
                            //show the choice
                            $showcount = $ccounter+1;
                            $str .= "Choice " . $showcount . " - " . $name;
                            if ($choice['cost'] >0)
                            {
                                $str .= "&pound;" . number_format($choice['cost'],2);
                            }
                            $str .= '<br />';
                            $ccounter +=1;
                        }
                        $str .= '</div>';
                    }
                    $str .= '</td>';
                    $str .= '<td>' . $item['number_of_items'] . '</td>';
                    $str .= '<td>&pound;' . $item['cost_single'] . '</td>';
                    $str .= '</tr>';
                }
                $str .= '</tbody>';
                $str .= '</table>';
                $str .= '<div id="basket-totals">';
                $str .= '<p>Total Items: ' . $totals['totitems'] . '</p>';
                $totalcost = $totals['totalcost'];
                if (($problem !== true) && ($deliverytype == 'D'))
                {
                    if (isset($deliveryam) && ($deliveryam != '0'))
                    {
                        $str .= '<p>Delivery Cost: &pound;' . number_format($deliveryam,2) . 'p</p>';
                        $totalcost = $totals['totalcost'] + $deliveryam;
                    }
                    elseif($deliveryam == 0)
                    {
                        $str .= '<p>Delivery Cost: FREE</p>';
                    }
                    if(isset($delmsg))
                    {
                        $str .= '<div class="alert alert-block alert-info">' . $delmsg . '</div>';
                    }
                }
                $str .= '<p>Total Cost: &pound;' . number_format($totalcost,2) . 'p</p>';
                $str .= '</div>';
            }
            $str .= '</div>';
            $str .= '<div class="modal-footer">';
            $str .= '<button class="btn" data-dismiss="modal" aria-hidden="true" data-loading-text="...">Cancel &times;</button>';
            if ($problem === true)
            {
                $str .= '<button type="submit" id="goback" data-loading-text="..." class="btn btn-success" title="Go Back">Go Back</button>';   
            }
            if (count($this->MenuItems) >= 1)
            {
                $str .= '<button type="submit" id="finalconfirm" data-loading-text="..." class="btn btn-success" title="Confirm Order">Continue';
                if ($problem === true)
                {
                    $str .= ' Anyway';
                }
                $str .= '</button>';
            }
            $str .= '</div>';

            return ($str);
        }

        function getSavedBasketContactInfo() {
            $fields = array(0 => 'firstname', 1 => 'surname', 2 => 'email', 3 => 'telno');
            $idfields = array(0 => 'basket_id');
            $idvals = array(0 => $this->ID);

            $DBA = new DatabaseInterface();
            $rs = $DBA->selectQuery(DBBASKETINFOTABLE, $fields, $idfields, $idvals);
            if ((!isset($rs[2])) && ($rs[1] != 0))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $retarray = $res;
                }
            }

            return $retarray;
        }

        function getBasketContactInfo() {
            $ret = $this->getSavedBasketContactInfo();

            $str = '';
            $str = '<div class="modal-header">';
            $str .= '<button class="close" data-dismiss="modal" aria-hidden="true">Cancel &times;</button>';
            $str .= '<h4 id="nextLabel">Your Contact Details</h4>';
            $str .= '</div>';
            $str .= '<div class="modal-body">';
            $str .= '<form name="ccontactinfo" id="ccontactinfo" class="form-horizontal">';
            $str .= '<h5>Please enter the information requested below. <small>All fields are required.</small></h5>';
            $str .= '<div class="control-group">';
            $str .= '<label class="control-label" for="cfname">First Name<small>*</small></label>';
            $str .= '<div class="controls"><input required="required" type="text" name="cfname" id="cfname" value="' . $ret[0] . '" placeholder="" /></div>';
            $str .= '</div>';
            $str .= '<div class="control-group">';
            $str .= '<label class="control-label" for="csname">Surname<small>*</small></label>';
            $str .= '<div class="controls"><input required="required" type="text" name="csname" id="csname" value="' . $ret[1] . '" placeholder="" /></div>';
            $str .= '</div>';
            $str .= '<div class="control-group">';
            $str .= '<label class="control-label" for="cemail">Email Address<small>*</small></label>';
            $str .= '<div class="controls"><input required="required" type="text" name="cemail" id="cemail" value="' . $ret[2] . '" placeholder="" /></div>';
            $str .= '</div>';
            $str .= '<div class="control-group">';
            $str .= '<label class="control-label" for="ctel">Telephone Number<small>*</small></label>';
            $str .= '<div class="controls"><input required="required" type="text" name="ctel" id="ctel" value="' . $ret[3] . '" placeholder="" /></div>';
            $str .= '</div>';
            $str .= '<div class="control-group">';
            $str .= '<label class="checkbox" for="acccreate"><input type="checkbox" name="acccreate" id="acccreate" value="1" checked="checked" />Please create me an account</label>';
            $str .= '</div>';
            $str .= '<div class="control-group" id="passcontrol">';
            $str .= '<label class="control-label" for="pass">Password for new account<small>*</small></label>';
            $str .= '<div class="controls"><input required="required" type="text" name="pass" id="pass" value="" /></div>';
            $str .= '</div>';
            $str .= '<div class="alert alert-info" id="accalert">';
            $str .= '<strong>Note!</strong> Full access to user accounts coming soon. Save your favourite meals, old orders and more!';
            $str .= '</div>';
            $str .= '</div>';
            $str .= '<div class="modal-footer">';
            $str .= '<button class="btn" data-dismiss="modal" aria-hidden="true" data-loading-text="...">Cancel &times;</button>';
            $str .= '<button type="submit" id="contprog" data-loading-text="..." class="btn btn-success" title="Confirm Order">Continue</button>';
            $str .= '</div>';
            $str .= '</form>';

            return($str);
        }

        function geoLocateCustomer($h,$s,$t,$c,$pc)
        {
            $mapaddress = urlencode($h . "+" . $s . "+" . $t . "+" . $c . "+" . $pc);
            $url = "http://maps.googleapis.com/maps/api/geocode/json?address=" . $mapaddress ."&sensor=false";
            // Retrieve the URL contents
            $page = file_get_contents($url);
            $resp = json_decode($page);
            $geoarray = array('Lat' => $resp->results[0]->geometry->location->lat,'Long' => $resp->results[0]->geometry->location->lng);

            return $geoarray;
        }
 
        function addUserInfo($h,$s,$t,$c,$pc,$dam,$dt)
        {
            $fields = array(0 => 'id');
            $idfields = array(0 => 'basket_id');
            $idvals = array(0 => $this->ID);
            $DBA = new DatabaseInterface();

            $rs = $DBA->selectQuery(DBBASKETINFOTABLE,$fields,$idfields,$idvals);

            if ((!isset($rs[2])) && ($rs[1] == 0))
            {
                $id = getmaxid(DBBASKETINFOTABLE);
                $fields = array(0 => 'id', 1 => 'basket_id', 2 => 'house', 3 => 'street', 4 => 'town', 5 => 'county', 6 => 'pcode', 7 => 'delamount', 8 => 'deltype');
                $fieldvals = array(0 => $id, 1 => $this->ID, 2 => $h, 3 => $s, 4 => $t, 5 => $c, 6 => $pc, 7 => $dam, 8 => $dt);
                $DBA2 = new DatabaseInterface();
                $rs2 = $DBA2->insertQuery(DBBASKETINFOTABLE, $fields, $fieldvals);

                unset($id,$fields,$fieldvals,$DBA2,$rs2);
            }
            unset($DBA, $fields,$idfields,$idvals);
            return true;
        }

        function updateBasketInfo($vals) {
            $DBA = new DatabaseInterface();
            $fields = array('id');
            $idfields = array(0 => 'basket_id');
            $idvals = array(0 => $this->ID);
            $rs = $DBA->selectQuery(DBBASKETINFOTABLE,$fields,$idfields,$idvals);

            if($rs[1] == 0)
            { 
                $found = false;
            }
            else
            {
                $found = true;
            }
            unset($fields,$fieldvals,$idfield,$idval,$DBA,$rs); 
            $fields = array();
            $fieldvals = array();
            foreach ($vals as $field => $val)
            {
                array_push($fields,$field);
                array_push($fieldvals, $val);
            }

            $DBA = new DatabaseInterface();
            if ($found == false)
            {
                $id = getmaxid(DBBASKETINFOTABLE);
                array_push($fields,'basket_id', 'id');
                array_push($fieldvals, $this->ID, $id);
                $rs = $DBA->insertQuery(DBBASKETINFOTABLE,$fields,$fieldvals);
            }
            else
            {
                $idfield = 'basket_id';
                $idval = $this->ID;
                $rs = $DBA->updateQuery(DBBASKETINFOTABLE,$fields,$fieldvals,$idfield,$idval);
            }
            unset($fields,$fieldvals,$idfield,$idval,$DBA,$rs);                
            return true;
        }

        function calculateDistanceToStore($coords,$storecoords) {
            $distance = distance_apart($coords, $storecoords,true);
            $distance = round($distance, 2);
            //$this->setUserVar('DistanceToStore', $distance);
            return $distance;
        }

        function prepareBasketPaymentForm($acflag) {
            //need choice if store paypal address is set then offer paypal otherwise its cash only
            $Store = new Store();
            $Store->getStore($this->StoreID);
            $Store->setStoreVar('SetLocationID', $this->LocationID);
            $Store->setStoreVar('Locations', $Store->getLocations());

            //start modal
            $str = '';
            $str = '<div class="modal-header">';
            $str .= '<button class="close" data-dismiss="modal" aria-hidden="true">Cancel &times;</button>';
            $str .= '<h4>Choose Payment Type</h4>';
            $str .= '</div>';
            $str .= '<div class="modal-body">';
            if ($acflag === true)
            {
                $str .= '<div class="alert alert-block alert-success">';
                $str .= '<strong>Success!</strong> Your new account has been created.';
                $str .= '</div>';
            }
            $str .= '<form name="deliveryinfo" id="deliveryinfo" class="form-horizontal">';
            $str .= '<div class="row-fluid">';
            if (isset($Store->Locations[$Store->SetLocationID]['ppadd']))
            {
                $str .= '<div class="control-group span3 offset1">';
                $str .= '<label class="radio">';
                $str .= '<input type="radio" name="pchoiceopt" id="pchoicepp" value="pp" checked>';
                $str .= 'Pay Via Paypal';
                $str .= '</label>';
                $str .= '</div>';
            }

            $str .= '<div class="control-group span3 offset1">';
            $str .= '<label class="radio">';
            $str .= '<input type="radio" name="pchoiceopt" id="pchoicec" value="c" checked>';
            $str .= 'Pay With Cash';
            $str .= '</label>';
            $str .= '</div>';

            if ($Store->Locations[$Store->SetLocationID]['pdq'] == 1)
            {
                $bsk = $this->getBasketInfo();
                //we have a pdq ability
                //what kinds of payments and what cards

                if ($Store->Locations[$Store->SetLocationID]['pdqphone'] == 1)
                {
                    //phone payments
                    $str .= '<div class="control-group span3 offset1">';
                    $str .= '<label class="radio">';
                    $str .= '<input type="radio" name="pchoiceopt" id="pchoicepbp" value="pbp">';
                    $str .= 'Pay via Phone';
                    $str .= '</label>';
            		$str .= '</div>';
                    //instore with card
                    if ($bsk['deltype'] === 'C')
                    {
                        $str .= '<div class="control-group span3 offset1">';
                        $str .= '<label class="radio">';
                        $str .= '<input type="radio" name="pchoiceopt" id="pchoicepbp" value="coc">';
                        $str .= 'Pay via Card on collection';
                        $str .= '</label>';
                        $str .= '</div>';
                    }
                }
                if (($Store->Locations[$Store->SetLocationID]['pdqmobile'] == 1) && ($bsk['deltype'] === 'D'))
                {
                    //mobile doorstep //only on delivery
                    $str .= '<div class="control-group span3 offset1">';
                    $str .= '<label class="radio">';
                    $str .= '<input type="radio" name="pchoiceopt" id="pchoicecod" value="cod">';
                    $str .= 'Pay via Card on delivery';
                    $str .= '</label>';
            $str .= '</div>';
                }
            }

            $str .= '</div>';
            if ($Store->Locations[$Store->SetLocationID]['pdq'] == 1)
            {

                $cardstr = '<ul>';
                if ($Store->Locations[$Store->SetLocationID]['mastercard'] == 1)
                {
                    $cardstr .= '<li>Mastercard</li>';
                }
                if ($Store->Locations[$Store->SetLocationID]['maestro'] == 1)
                {
                    $cardstr .= '<li>Maestro</li>';
                }
                if ($Store->Locations[$Store->SetLocationID]['visael'] == 1)
                {
                    $cardstr .= '<li>Visa Electron</li>';
                }
                if ($Store->Locations[$Store->SetLocationID]['visa'] == 1)
                {
                    $cardstr .= '<li>Visa</li>';
                }
                if ($Store->Locations[$Store->SetLocationID]['solo'] == 1)
                {
                    $cardstr .= '<li>Solo</li>';
                }
                if ($Store->Locations[$Store->SetLocationID]['switch'] == 1)
                {
                    $cardstr .= '<li>Switch</li>';
                }
                $cardstr .= '</ul>';
                $str .= '<div class="alert alert-info">We currently accept the following cards ' . $cardstr . '</div>';
            }
            $str .= '</div>';
            $str .= '<div class="modal-footer">';
            $str .= '<button class="btn" data-dismiss="modal" aria-hidden="true" data-loading-text="...">Cancel &times;</button>';
            $str .= '<button type="submit" id="pchoiceprog" data-loading-text="..." class="btn btn-success" title="Continue">Continue</button>';
            $str .= '</div>';
            $str .= '</form>';

            return $str;
        }

        function preparePaypalForm($basketinfo) {
            if ($this->MenuItems == '')
            {
                $this->getBasketItems();
            }

            $Store = new Store();
            $Store->getStore($this->StoreID);
            $Store->getLocations();
            $Store->setStoreVar('SetLocationID', $this->LocationID);

            $itcount = 0;
            $ittotcost = 0;
            $str = '';
            $str = '<div class="modal-header">';
            $str .= '<button class="close" data-dismiss="modal" aria-hidden="true">Cancel &times;</button>';
            $str .= '<h4>Proceed to Paypal</h4>';
            $str .= '</div>';
            $str .= '<div class="modal-body">';
            $str .= '<div class="alert alert-warning alert-block"><strong>Warning</strong><p>When you click the button below you will be redirected to the Paypal website to make a payment.</p><p>Once finished please click the "Return to merchant" button so we can say thank you.</p></div>';
            $str .= '<form action="https://www.paypal.com/uk/cgi-bin/webscr" method="post" name="paypalform" id="paypalform">';
            $str .= '<input type="hidden" name="business" value="' . $Store->Locations[$Store->SetLocationID]['ppadd'] . '" />';
            $str .= '<input type="hidden" name="cmd" value="_xclick" />';
            foreach($this->MenuItems as $item)
            {
                $ittotcost += $item['cost_single'] * $item['number_of_items'];
                /*$itname = $this->getItemName($item['item_id']);
                if ($item['multiprice_description'] != '')
                {
                    $itname .= ' - ' . $item['multiprice_description'];
                }*/

                if ($item['hasextras'] == "Y")
                {
                    //prepare extras string
                    //$itname .= ' with extras';
                    $extraslist = $this->getExtrasList($item['id']);
                    foreach ($extraslist as $extra)
                    {
                        if ($extra['cost'] > 0)
                        {
                            $ittotcost += ($extra['cost'] * $item['number_of_items']);
                        }
                    }
                }
                if ($item['haschoices'] == "Y")
                {
                    //prepare choices string
                    //$itname .= ' with choices';
                    $choicelist = $this->getChoicesList($item['id']);
                    $Menu = new Menu();
                    $Menu->setMenuVar("ID", $this->MenuID);
                    $Menu->setMenuVar("StoreID",$this->StoreID);
                    $Menu->setMenuVar("LocationID", $this->LocationID);

                    $chtype = $Menu->getItemChoiceType($item['item_id']);
                    
                    $choiceparts = explode('/', $chtype);
                    foreach ($choicelist as $choice)
                    {
                        switch ($choiceparts[0]) {
                        case "X":
                            $name = $this->getChoiceInformation($item['id'], $choice['choice_id']);
                            break;
                        case "C":
                            $name = $Menu->getItemName($choice['choice_id']);
                            break;
                        }
                        //show the choice
                        if ($choice['cost'] >0)
                        {
                            $ittotcost += ($choice['cost'] * $item['number_of_items']);
                        }
                    }
                }
                $itcount++;
            }
            $Store->getStore();
            $itname = $Store->Name . ' Online Food Order';
            if ($basketinfo[4] = "D")
            {
                $str .= '<input type="hidden" name="address1" value="' . $basketinfo['del'][0] . ' ' . $basketinfo['del'][1] . '" />';
                $str .= '<input type="hidden" name="city" value="' . $basketinfo['del'][2] . '" />';
                $str .= '<input type="hidden" name="state" value="' . $basketinfo['del'][3] . '" />';
                $str .= '<input type="hidden" name="country" value="UK" />';
                $str .= '<input type="hidden" name="zip" value="' . $basketinfo['del'][4] . '" />';
                $ittotcost += $basketinfo['del'][5];
            }
            
            $str .= '<input type="hidden" name="item_name" value="' . $itname . '" />';
            //$str .= '<input type="hidden" name="number" value="' . $item['number_of_items'] . '" />';
            $str .= '<input type="hidden" name="amount" value="' . number_format($ittotcost, 2) . '" />';

            
            $str .= '<input type="hidden" name="email" value="' . $basketinfo['contact'][2] . '" />';
            $str .= '<input type="hidden" name="first_name" value="' . $basketinfo['contact'][0] . '" />';
            $str .= '<input type="hidden" name="last_name" value="' . $basketinfo['contact'][1] . '" />';
            $str .= '<input type="hidden" name="night_phone_b" value="' . $basketinfo['contact'][3] . '"/>';
            $str .= '<input type="hidden" name="no_shipping" value="0" />';
            $str .= '<input type="hidden" name="currency_code" value="GBP" />';
            $str .= '<input type="hidden" name="handling" value="' . number_format($this->DeliveryAmount,2) . '" />';
            $str .= '<input type="hidden" name="return" value="'. SITE_PATH . 'paypal-thankyou/' . $this->ID . '/" />';
            $str .= '<input type="hidden" name="cancel_return" value="'. SITE_PATH . 'paypal-cancel/' . $this->ID . '/" />';
            $str .= '<input type="hidden" name="notify_url" value="'. SITE_PATH . 'paypal-callback.php?bid=' . $this->ID . '" />';
            $str .= '<button class="btn btn-success" type="submit" id="proceed-secure-payment-button" name="proceed_to_payment">Proceed and Pay</button>';

            $str .= '</div>';
            $str .= '<div class="modal-footer">';
            $str .= '<button class="btn" data-dismiss="modal" aria-hidden="true" data-loading-text="...">Cancel &times;</button>';
            //$str .= '<button type="submit" id="pchoiceprog" data-loading-text="..." class="btn btn-success" title="Continue">Continue</button>';
            $str .= '</form>';
            $str .= '</div>';
            return $str;
        }

        function getAllInfo() {
            $this->getBasketItems();
            $ret = $this->getBasketInfo();
            $ret2 = $this->getSavedBasketContactInfo();
            $returnarray = array('contact' => $ret2, 'del' => $ret);
            return $returnarray;
        }

        function convert2Order($extinf) {
            $reference = createorderref();

            $id = getmaxid(DBORDERHEADERTABLE);
            $fields = array(0 => 'user_id', 1 => 'store_id', 2 => 'location_id', 3 => 'menu_id', 4 => 'discount', 5 => 'deliverytype', 6 => 'deliverycost', 7 => 'id', 8 => 'reference', 9 => 'time', 10 => 'printer_message', 11 => 'contactemail', 12 => 'paymenttype', 13 => 'payment_status', 14 => 'contacttelephone');
            $fieldvals = array(0 => $this->UserID, 1 => $this->StoreID, 2 => $this->LocationID, 3 => $this->MenuID, 4 => $this->Discount, 5 => $this->DeliveryType, 6 => $this->DeliveryAmount, 7 => $id, 8 => $reference, 9 => time(), 10 => $printer_message, 11 => $this->ContactEmail, 12 => $this->PaymentType, 13 => $this->PaymentStatus, 14 => $this->Contel);
            if ($this->DeliveryType == 'D')
            {
                array_push($fields, 'house', 'street', 'town', 'county', 'pcode');
                array_push($fieldvals, $this->House, $this->Street, $this->Town, $this->County, $this->PCode);
            }
            $DBA = new DatabaseInterface();
            $rs = $DBA->insertQuery(DBORDERHEADERTABLE, $fields, $fieldvals);

            if ($rs == 1)
            {
                //create items
                foreach ($this->MenuItems as $item)
                {
                    $DBA2 = new DatabaseInterface();
                    $fields2 = array(0 => 'name');
                    $idfields2 = array(0 => 'id', 1 => 'menu_id');
                    $idvals2 = array(0 => $item['item_id'], 1 => $this->MenuID);
                    $rs2 = $DBA2->selectQuery(DBOLMENUITEMSTABLE, $fields2, $idfields2, $idvals2);
                    if (!isset($rs[2]))
                    {
                        while ($res2 = mysql_fetch_array($rs2[0]))
                        {
                            $name = $res2['name'];
                        }
                    }
                    unset($DBA2, $rs2, $res2, $fields2, $idfields2, $idvals2);

                    //copy $items to order items table
                    $iid = getmaxid(DBORDERITEMSTABLE);
                    $fields = array(0 => 'id', 1 => 'order_id', 2 => 'item_id', 3 => 'number_of_items', 4 => 'cost_single', 5 => 'name', 6 => 'multiprice_description', 7 => 'hasextras', 8 => 'haschoices');
                    $fieldvals = array(0 => $iid, 1 => $id, 2 => $item['item_id'], 3 => $item['number_of_items'], 4 => $item['cost_single'], 5 => $name, 6 => $item['multiprice_description'], 7 => $item['hasextras'], 8 => $item['haschoices']);

                    $DBA2 = new DatabaseInterface();
                    $rs2 = $DBA2->insertQuery(DBORDERITEMSTABLE, $fields, $fieldvals);

                    if (!$rs2 == 1)
                    {
                        $this->setBasketVar('Error', 'Error creating item');
                    }

                    unset($fields, $fieldvals, $DBA2, $rs2);

                    if ($item['hasextras'] == "Y")
                    {
                        //copy extras for this item into order items extra table
                        $extraslist = $this->getExtrasList($item['id']);
                        if (count($extraslist) >= 1)
                        {
                            foreach($extraslist as $extra)
                            {
                                $eid = getmaxid(DBORDERITEMEXTRASTABLE);
                                $extraname = $this->getExtraName($item['item_id'], $extra['extra_id']);
                                $fields3 = array(0 => 'id', 1 => 'order_id', 2 => 'order_table_id', 3 => 'item_id', 4 => 'extra_id', 5 => 'cost', 6 => 'name');
                                $fieldvals3 = array(0 => $eid, 1 => $id, 2 => $iid, 3 => $item['item_id'], 4 => $extra['extra_id'], 5 => $extra['cost'], 6 => $extraname);
                                $DBA3 = new DatabaseInterface();
                                $rs3 = $DBA3->insertQuery(DBORDERITEMEXTRASTABLE, $fields3, $fieldvals3);
                                if (!$rs3 == 1)
                                {
                                    $this->setBasketVar('Error', 'Error adding extra ' . $extra['name']);
                                }
                                unset($rs3, $fieldvals3, $fields3, $DBA3, $eid);
                            }
                        }
                    }
                    if ($item['haschoices'] == "Y")
                    {
                        $choiceslist = $this->getChoicesList($item['id']);
                        if (count($choiceslist) >= 1)
                        {
                            $choicetype = $this->getItemChoiceType($item['item_id']);
                            $choiceparts = explode('/', $choicetype);
                            foreach($choiceslist as $choice)
                            {
                                $cid = getmaxid(DBORDERITEMCHOICESTABLE);
                                if($choiceparts[0] == 'X')
                                {
                                    $choiceinfo = $this->getChoiceInformation($item['item_id'], $choice['choice_id']);
                                    $name = $choiceinfo['name'];
                                    $cost = number_format($choiceinfo['cost'], 2);
                                }
                                else
                                {
                                    $cost = number_format(0,2);
                                    $name = $this->getItemName($choice['choice_id']);
                                }
                                $fields3 = array(0 => 'id', 1 => 'order_id', 2 => 'order_table_id', 3 => 'item_id', 4 => 'choice_id', 5 => 'cost', 6 => 'name');
                                $fieldvals3 = array(0 => $cid, 1 => $id, 2 => $iid, 3 => $item['item_id'], 4 => $choice['choice_id'], 5 => $choice['cost'], 6 => $name);
                                $DBA3 = new DatabaseInterface();
                                $rs3 = $DBA3->insertQuery(DBORDERITEMCHOICESTABLE, $fields3, $fieldvals3);
                                if (!$rs3 == 1)
                                {
                                    $this->setBasketVar('Error', 'Error adding extra choices.');
                                }
                                unset($rs3, $fieldvals3, $fields3, $DBA3, $eid);
                            }
                        }
                    }
                }
                //$this->DeleteBasket();
                return($reference);
            }
            else
            {
                $this->setBasketVar('Error', "Error saving header record");

            }
        }

        function getDeliveryAmount() {
            $fields = array(0 => 'delamount');
            $idfields = array(0 => 'basket_id');
            $idvals = array(0 => $this->ID);
            $DBA = new DatabaseInterface();
            $rs = $DBA->selectQuery(DBBASKETINFOTABLE,$fields,$idfields,$idvals);
            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $delam = $res['delamount'];
                }
            }
            unset($rs,$res,$fields,$idfields,$idvals,$DBA);

            return $delam;
        }

        function processAsOrder($pchoice) {
            $ret = $this->getAllInfo();
            if ($ret['deltype'] == 'D')
            {
                $delam = $this->getDeliveryAmount();
                $this->setBasketVar('DeliveryAmount', $delam);
                $this->setBasketVar('DeliveryType', $ret['deltype']);
                $this->setBasketVar('House', $ret['del']['house']);
                $this->setBasketVar('Street', $ret['del']['street']);
                $this->setBasketVar('Town', $ret['del']['town']);
                $this->setBasketVar('County', $ret['del']['county']);
                $this->setBasketVar('PCode', $ret['del']['pcode']);
            }
            else
            {
                $this->setBasketVar('DeliveryAmount', 0.00);
                $this->setBasketVar('DeliveryType', 'C');
            }
            switch ($pchoice) {
                case "c":
                    $status = "NOT PAID";
                    $pchoicefull = 'cash';
                    break;
                case "paypal":
                    $status = "PAID";
                    $pchoicefull = 'paypal';
                    break;
                case "pbp":
                    $status = "NOT PAID";
                    $pchoicefull = 'phone';
                    break;
                case "coc":
                    $status = "NOT PAID";
                    $pchoicefull = 'card on collection';
                    break;
                case "cod":
                    $status = "NOT PAID";
                    $pchoicefull = 'card on delivery';
                    break;
            }
            $this->setBasketVar('PaymentStatus', $status);
            $this->setBasketVar('PaymentType', $pchoicefull);
            $this->setBasketVar('ContactEmail',$ret['contact']['email']);
            $this->setBasketVar('FirstName',$ret['contact']['firstname']);
            $this->setBasketVar('Surname',$ret['contact']['surname']);
            $this->setBasketVar('Contel',$ret['contact']['telno']);

            $orderid = $this->convert2Order($ret);//returns table id of new order

            $this->sendOrderMail($orderid);
            //return html string conmfirming sent and telling what to do

            if ($pchoicefull != 'paypal')
            {
                $str = '';
                $str = '<div class="modal-header">';
                $str .= '<button class="close" data-dismiss="modal" aria-hidden="true">Cancel &times;</button>';
                $str .= '<h4>Thank you!</h4>';
                $str .= '</div>';
                $str .= '<div class="modal-body">';
                if ($pchoicefull === 'phone')
                {
                    $str .= '<div class="alert alert-block alert-info">Please ring the phone number at the top of the home page to make payment quoting Order Reference:- ' . $orderid .'</div>';                    
                }
                $str .= '<div class="alert alert-block alert-success">Your order was successfully sent to the store, you should also receive an email confirming your order to the email address you provided. If you have any questions at all please call the phone number listed at the top of the main page.</div>';
                $str .= '</div>';
                $str .= '<div class="modal-footer">';
                $str .= '<button class="btn" id="finito" data-dismiss="modal" aria-hidden="true" data-loading-text="...">Finish &times;</button>';
                $str .= '</div>';
                return $str;
            }
            else
            {
                return $orderid;
            }
        }

        function updateUserID($custid) {
            $fields = array(0 => 'user_id');
            $fieldvals = array(0 => $custid);
            $idfields = array(0 => 'id');
            $idvals = array(0 => $this->ID);

            $DBA = new DatabaseInterface();
            $rs = $DBA->updateQuery(DBBASKETTABLE,$fields,$fieldvals,$idfields,$idvals);
        }

        function sendOrderMail($orderid) {
            $Store = new Store();
            $Store->setStoreVar('ID', $this->StoreID);
            $Store->getStore();
            $Store->setStoreVar('SetLocationID', $this->LocationID);

            $this->createMessage($orderid,'email');
            $sub = "Order from your website";
            $this->sendMessage($Store->Locations[$Store->SetLocationID]['orderemail'],$this->Message,$sub,$Store->Locations[$Store->SetLocationID]['contactemail']);

            $custmess = "Thanks for your order!\n\nYour order has been sent to " . SITE_NAME . " and if there are any problems with your order they will contact you on the information you provided.\n\n";
            $custmess .= "If you have any queries then please do not hesitate to contact us on " . $Store->Locations[$Store->SetLocationID]['contactphone'] . ".\n\nYour order is listed below for your information.\n\n";
            $custmess .= $this->Message;
            $sub = "Your order at " . SITE_NAME;
            $this->sendMessage($this->ContactEmail,$custmess,$sub,$Store->Locations[$Store->SetLocationID]['contactemail']);
            unset($Store);
        }

        function createMessage($finalordno,$messagetype) {
            /*unset($first_time_order);
            if (!isset($newUser))
            {
                $first_time_order = true;
            }
            else
            {
                $this->setBasketVar('UserID', $newUser->ID);
                $this->checkFirstTimeOrder();
            }*/
            $messagetostore .= "Order Reference: " . $finalordno . "\n";
            $messagetostore .= "Date: " . date("H:i d-m-Y") . "\n";
            $messagetostore .= "To: " . SITE_NAME . "\n";
            /*if ((isset($first_time_order)) || ($this->FTO == 'true'))
            {
                $messagetostore .= "***FIRST TIME ORDER***\n";
            }*/
            $messagetostore .= "--------------------------------\n\n";
            $messagetostore .= "Customer Info:\n";
            $messagetostore .= $this->FirstName . " " . $this->Surname . "\n";
            if ($this->DeliveryType == 'D')
            {
                $messagetostore .= $this->House . " " . $this->Street . ",\n";
                $messagetostore .= $this->Town . ", " . $this->County . " " . $this->PCode . "\n\n";
            }
            $messagetostore .= "Contact Telephone: " . $this->Contel . "\n";
            $messagetostore .= "Email Address: " . $this->ContactEmail . "\n";
            /*if (isset($newUser->Mobile))
            {
                $messagetostore .= 'Mobile: ' . $newUser->Mobile . "\n";
            }

            if (!isset($newUser))
            {
                $messagetostore .= 'Email address: ' . $_POST['newuemail'] . "\n";
                $this->setBasketVar('ContactEmail', $_POST['newuemail']);
            }
            else
            {
                $messagetostore .= 'Email address: ' . $newUser->ContactEmail . "\n";
                $this->setBasketVar('ContactEmail', $newUser->ContactEmail);
            }*/
            $messagetostore .= "--------------------------------\n";
            if ($this->DeliveryType == "D"){
                $dtype = "Delivery";
            } else {
                $dtype = "Collection";
            }

            $messagetostore .= "Delivery or Collection: " . $dtype . "\n";
            $messagetostore .= "Payment Type: " . $this->PaymentType . "\n";
            $messagetostore .= $this->PaymentStatus . "\n";
            $messagetostore .= "--------------------------------\n";
            $messagetostore .= "Item Information\n";
            $counter = 0;

            foreach ($this->MenuItems as $item)
            {
                $messagetostore .= $item['number_of_items'] . " x ";
                $DBA = new DatabaseInterface();
                $fields = array(0 => 'name');
                $idfields = array(0 => 'id', 1 => 'menu_id');
                $idvals = array(0 => $item['item_id'], 1 => $this->MenuID);

                $rs = $DBA->selectQuery(DBOLMENUITEMSTABLE, $fields, $idfields, $idvals);

                if (!isset($rs[2]))
                {
                    while ($res = mysql_fetch_array($rs[0]))
                    {
                        $name = $res['name'];
                    }
                }
                $messagetostore .= ucwords($name);
                if (isset($item['multiprice_description']))
                {
                    $messagetostore .= " - " . $item['multiprice_description'];
                }
                $messagetostore .= " \n";
                $excost = 0;
                if ($item['hasextras'] == "Y")
                {
                    //prepare extras string
                    $extraslist = $this->getExtrasList($item['id']);
                    $messagetostore .= "With:- \n";
                    foreach ($extraslist as $extra)
                    {

                        $name = $this->getExtraName($item['item_id'],$extra['extra_id']);
                        $messagetostore .= $name;
                        /*if ($extra['cost'] > 0)
                        {
                            $messagetostore .= ' �' . number_format($extra['cost'],2);
                        }*/
                        $messagetostore .= "\n";
                    }
                }

                if ($item['haschoices'] == "Y")
                {
                    //prepare choices string
                    $choicelist = $this->getChoicesList($item['id']);
                    //$messagetostore .= "Choices:- \n";
                    $Menu = new Menu();
                    $Menu->setMenuVar("ID", $this->MenuID);
                    $Menu->setMenuVar("StoreID",$this->StoreID);
                    $Menu->setMenuVar("LocationID", $this->LocationID);

                    $chtype = $Menu->getItemChoiceType($item['item_id']);
                    
                    $choiceparts = explode('/', $chtype);
                    $ccounter = 0;
                    foreach ($choicelist as $choice)
                    {
                        switch ($choiceparts[0]) {
                        case "X":
                            $ret = $this->getChoiceInformation($item['item_id'], $choice['choice_id']);
                            $name = $ret['name'];
                            break;
                        case "C":
                            $name = $Menu->getItemName($choice['choice_id']);
                            break;
                        }
                        //show the choice
                        $showcount = $ccounter+1;
                        $messagetostore .= "Choice " . $showcount . " - " . $name;
                        /*if ($choice['cost'] >0)
                        {
                            $messagetostore .= "£" . number_format($choice['cost'],2);
                        }*/
                        $messagetostore .= "\n";
                        $ccounter +=1;
                    }
                }
                $messagetostore .= "\n";
                $totalprice += $item['number_of_items'] * $item['cost_single'];
                unset($fields, $idfields, $idvals, $DBA, $rs, $res);
                $counter += $item['number_of_items'];
            }
            if (isset($_POST['ext-info']))
            {
                $message = strip_fuck(extra_strip_tags_attributes(prevent_inject($_POST['ext-info'])));
                $messagetostore .= "Extra Info\n";
                $messagetostore .= $message . "\n";
                $messagetostore .= "--------------------------------\n";
            }
            $messagetostore .= "\nTotal Items: " . $counter . "\n";
            if ($this->DeliveryChoice == "D")
            {
                $messagetostore .= "Delivery Total: £" . $this->DeliveryAmount . "\n";
                $totalprice += $this->DeliveryAmount;
            }
            $messagetostore .= "\nTotal price: £" . number_format($totalprice,2) . "\n";
            /*$messagetostore2 = urlencode($messagetostore);
            $result = $Store->sendMessage($messagetostore2);*/
            $this->setBasketVar('Message', $messagetostore);
        }

        function sendMessage($to,$mailcontent,$subject,$from) {
            $headers = "From: " . $from . "\r\nReply-to: " . $_GET['email'] . "\r\n";
            $headers .= "MIME-Version: 1.0" . "\r\n" . "Content-type: text/plain; charset=UTF-8" . PHP_EOL;
            mail($to, $subject, $mailcontent, $headers);
        }

        function updateStatus($type) {
            switch ($type) {
                case "ppreturned":
                case "cusreturned":
                    $this->UPDStatus($type);
                    break;
                case "Both":
                    $this->UPDStatus("ppreturned");
                    $this->UPDStatus("cusreturned");
                    break;
            }
        }

        function UPDStatus($var) {
            $fields = array(0 => $var);
            $fieldvals = array(0 => 1);
            $idfields = 'id';
            $idvals = $this->ID;
            $DBA = new DatabaseInterface();

            $rs = $DBA->updateQuery(DBBASKETTABLE,$fields,$fieldvals,$idfields,$idvals);
        }
    }
?>
