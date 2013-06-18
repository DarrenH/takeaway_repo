<?php
    Class Order {
        var $ID,
        $UserID,
        $StoreID,
        $LocationID,
        $MenuID,
        $Reference,
        $Items,
        $Discount,
        $ExtrasCost,
        $ChoicesCost,
        $DeliveryCost,
        $TotalPrice,
        $DeliveryOrCollection,
        $StatusText,
        $Status,
        $Archived,
        $Time,
        $ReadyTime,
        $House,
        $Street,
        $Town,
        $County,
        $PCode,
        $Contel,
        $FormattedAddress,
        $PaymentType,
        $PaymenStatus;

        function Order () {

        }

        function setOrderVar($varname,$varvalue)
        {
            $this->$varname = $varvalue;
        }

        function getOrder($oID = null)
        {
            if (isset($oID))
            {
                $this->setOrderVar('ID', $oID);
            }

            $DBA = new DatabaseInterface();
            $fields = array(0 => 'id', 1 => 'user_id', 2 => 'store_id', 3 => 'menu_id', 4 => 'time', 5 => 'location_id', 6 => 'reference', 7 => 'discount', 8 => 'deliverycost', 9 => 'deliverytype', 10 => 'status', 11 => 'ready_time', 12 => 'house', 13 => 'street', 14 => 'town', 15 => 'county', 16 => 'pcode', 17 => 'contacttelephone', 18 => 'paymenttype', 19 => 'payment_status', 14 => 'archived');
            if (isset($oID))
            {
                $idfields = array(0 => 'id');
                $idvals = array(0 => $this->ID);
            }
            else
            {
                unset($idfields, $idvals);
            }

            $rs = $DBA->selectQuery(DBORDERHEADERTABLE, $fields, $idfields, $idvals);

            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $this->setOrderVar('ID', $res['id']);
                    $this->setOrderVar('UserID', $res['user_id']);
                    $this->setOrderVar('StoreID', $res['store_id']);
                    $this->setOrderVar('LocationID', $res['location_id']);
                    $this->setOrderVar('MenuID', $res['menu_id']);
                    $this->setOrderVar('Reference', $res['reference']);
                    $this->setOrderVar('Discount', $res['discount']);
                    $this->setOrderVar('DeliveryCost', $res['deliverycost']);
                    if ($res['deliverytype'] == 'C')
                    {
                        $this->setOrderVar('DeliveryOrCollection', 'C');
                    }
                    else
                    {
                        $this->setOrderVar('DeliveryOrCollection', 'D');
                    }
                    $this->setOrderVar('Time', $res['time']);
                    $this->setOrderVar('Status', $res['status']);
                    $this->setOrderVar('ReadyTime', $res['ready_time']);
                    $this->setOrderVar('House', $res['house']);
                    $this->setOrderVar('Street', $res['street']);
                    $this->setOrderVar('Town', $res['town']);
                    $this->setOrderVar('County', $res['county']);
                    $this->setOrderVar('PCode', $res['pcode']);
                    $this->setOrderVar('Contel', $res['contacttelephone']);
                    $this->setOrderVar('PaymentType', $res['paymenttype']);
                    $this->setOrderVar('PaymentStatus', $res['payment_status']);
                    $this->setOrderVar('Archived', $res['archived']);
                    $this->getStatusText();
                    $this->getOrderItems();
                    $this->getExtrasCost();
                    $this->getChoicesCost();
                }
            }

            unset($rs, $fields, $idfields, $idvals, $DBA, $res);
        }

        function getOrderItems()
        {
            $items = array();
            $fields = array(0 => 'id', 1 => 'item_id', 2 => 'number_of_items', 3 => 'cost_single', 4 => 'hasextras', 5 => 'multiprice_description', 6 => 'haschoices');
            $idfields = array(0 => 'order_id');
            $idvals = array(0 => $this->ID);

            $DBA = new DatabaseInterface();
            $rs = $DBA->selectQuery(DBORDERITEMSTABLE, $fields, $idfields, $idvals);
            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    array_push($items, $res);
                }
            }

            $this->setOrderVar('Items', $items);

            unset($rs, $fields, $idfields, $idvals, $DBA, $res, $items);
        }

        function updateOrderStatus($status)
        {
            $fields = array(0 => 'status');
            $fieldvals = array(0 => $status);
            $idfields = 'id';
            $idvals = $this->ID;

            $DBA = new DatabaseInterface();
            $rs = $DBA->updateQuery(DBORDERHEADERTABLE, $fields, $fieldvals, $idfields, $idvals);
            unset($fields, $fieldvals, $idfields, $idvals, $rs, $DBA);
        }

        function updateOrderArchived($status)
        {
            $fields = array('archived');
            $fieldvals = array(0 => $status);
            $idfields = 'id';
            $idvals = $this->ID;

            $DBA = new DatabaseInterface();
            $rs = $DBA->updateQuery(DBORDERHEADERTABLE, $fields, $fieldvals, $idfields, $idvals);
            unset($fields, $fieldvals, $idfields, $idvals, $rs, $DBA);
        }

        function updateDeliveryTime($timestamp)
        {
            $fields = array(0 => 'ready_time');
            $fieldvals = array(0 => $timestamp);
            $idfields = 'id';
            $idvals = $this->ID;

            $DBA = new DatabaseInterface();
            $rs = $DBA->updateQuery(DBORDERHEADERTABLE, $fields, $fieldvals, $idfields, $idvals);
            unset($fields, $fieldvals, $idfields, $idvals, $rs, $DBA);
            $this->setOrderVar('ReadyTime', $timestamp);
        }

        function updateProcessedTime($timestamp)
        {
            $fields = array(0 => 'processed_time');
            $fieldvals = array(0 => $timestamp);
            $idfields = 'id';
            $idvals = $this->ID;

            $DBA = new DatabaseInterface();
            $rs = $DBA->updateQuery(DBORDERHEADERTABLE, $fields, $fieldvals, $idfields, $idvals);
            unset($fields, $fieldvals, $idfields, $idvals, $rs, $DBA);
        }

        function getTotalPrice()
        {
            if (count($this->Items) >= 1)
            {
                foreach ($this->Items as $item)
                {
                    $total_price += $item['number_of_items'] * $item['cost_single'];
                }
                $total_price += $this->ExtrasCost;
                $total_price += $this->ChoicesCost;
                $total_price += $this->DeliveryCost;
                $this->setOrderVar('TotalPrice', number_format($total_price, 2));
            }
            else
            {
                $this->setOrderVar('TotalPrice', number_format(0, 2));
            }
        }

        function getStatusText()
        {
            switch ($this->Status) {
                case "U":
                    $text = "Unconfirmed";
                    break;
                case "P":
                    $text = "Supplier has responded";
                    break;
                case "F":
                    $text = "Completed";
                    break;
                case "X":
                    $text = "Refused";
            }
            $this->setOrderVar('StatusText', $text);
        }

        function getExtrasList($otid)
        {
            $returnarray = array();

            $DBA = new DatabaseInterface();
            $fields = array(0 => '*');
            $idfields = array(0 => 'order_id', 1 => 'order_table_id');
            $idvals = array(0 => $this->ID, 1 => $otid);

            $rs = $DBA->selectQuery(DBORDERITEMEXTRASTABLE, $fields, $idfields, $idvals);

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

        function getExtraInformation($iid, $extid)
        {
            $DBA3 = new DatabaseInterface();
            $fields3 = array(0 => '*');
            $idfields3 = array(0 => 'item_id', 1 => 'order_id', 2 => 'extra_id');
            $idvals3 = array(0 => $iid, 1 => $this->ID, 2 => $extid);
            $rs3 = $DBA3->selectQuery(DBORDERITEMEXTRASTABLE, $fields3, $idfields3, $idvals3);
            if ((!isset($rs3[2])) && ($rs3[1] >= 1))
            {
                while ($res3 = mysql_fetch_array($rs3[0]))
                {
                    $retname = $res3;
                }
            }

            unset($DBA3, $fields3, $idfields3, $idvals3, $rs3, $res3);

            return $retname;
        }

        function formatAddress($type = null)
        {
            $fa = $this->House . ' ';
            $fa .= $this->Street;
            if ($type=='print')
            {
                $fa .= ',\n';
            }
            else
            {
                $fa .= ',<br />';
            }
            $fa .= $this->Town;
            if($type=='print')
            {
                $fa .= ',\n';
            }
            else
            {
                $fa .= ',<br />';
            }
            $fa .= $this->County;
            $fa .= '<br />' . $this->PCode;
            if ($type=='print')
            {
                $this->setOrderVar('FormattedPrintAddress', $fa);
            }
            else
            {
                $this->setOrderVar('FormattedAddress', $fa);
            }
        }

        function getExtrasCost()
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'sum(cost) as extcost');
            $idfields = array(0 => 'order_id');
            $idvals = array(0 => $this->ID);

            $rs = $DBA->selectQuery(DBORDERITEMEXTRASTABLE, $fields, $idfields, $idvals);

            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $this->setOrderVar('ExtrasCost', number_format($res['extcost'],2));
                }
            }
            unset($rs, $res, $fields, $idfields, $idvals, $DBA);
        }

        function getChoicesCost()
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'sum(cost) as extcost');
            $idfields = array(0 => 'order_id');
            $idvals = array(0 => $this->ID);

            $rs = $DBA->selectQuery(DBORDERITEMCHOICESTABLE, $fields, $idfields, $idvals);

            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $this->setOrderVar('ChoicesCost', number_format($res['extcost'],2));
                }
            }
            unset($rs, $res, $fields, $idfields, $idvals, $DBA);
        }

        function getItemChoiceType($iid)
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'choice_type');
            $idfields = array(0 => 'menu_id', 1 => 'id');
            $idvals = array(0 => $this->ID, 1 => $iid);

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

        function getChoicesList($btid)
        {
            $returnarray = array();

            $DBA = new DatabaseInterface();
            $fields = array(0 => '*');
            $idfields = array(0 => 'order_id', 1 => 'order_table_id');
            $idvals = array(0 => $this->ID, 1 => $btid);

            $rs = $DBA->selectQuery(DBORDERITEMCHOICESTABLE, $fields, $idfields, $idvals);

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

        function notifyUser()
        {
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'contactemail', 1 => 'first_name', 2 => 'last_name');
            $idfields = array(0 => 'id');
            $idvals = array(0 => $this->UserID);

            $rs = $DBA->selectQuery(DBUSERTABLE, $fields, $idfields, $idvals);

            if ((!isset($rs[2])) && ($rs[1] >= 1))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $mail = $res['contactemail'];
                    $name = $res['first_name'] . ' ' . $res['last_name'];
                    if ($this->DeliveryorCollection == 'C')
                    {
                        $delcol = 'Collection';
                    }
                    else
                    {
                        $delcol = 'Delivery';
                    }

                    $DBA2 = new DatabaseInterface();

                    $fields2 = array(0 => 'name');
                    $idfields2 = array(0 => 'id');
                    $idvals2 = array($this->StoreID);

                    $rs2 = $DBA2->selectQuery(DBSTORETABLE, $fields2, $idfields2, $idvals2);

                    if (!isset($rs2[2]))
                    {
                        while ($res2 = mysql_fetch_array($rs2[0]))
                        {
                            $storename = $res2['name'];
                        }
                    }

                    unset($DBA2, $fields2, $idfields2, $idvals2, $rs2, $res2);

                    $arr = array('order-ref' => $this->Reference, 'readytime' => date("H:i", $this->ReadyTime), 'viewlink' => SITE_PATH . 'view-order/' . $this->Reference . '/', 'del-col' => $delcol, 'storename' => $storename, 'status' => $this->Status);
                }
            }

            unset($DBA, $rs, $res, $fields, $idfields, $idvals);

            sendemail($mail,$name,'OrderUpdate', $arr);
        }
    }
?>