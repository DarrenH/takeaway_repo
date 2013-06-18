<?php
    function getPages ()
    {
        $DBA = new DatabaseInterface();
        $pages = array();

		$fields = array(0 => 'id', 1 => 'name', 2 => 'description', 3 => 'templatefile');
        $orderby = 'id asc';
		$rs = $DBA->selectQuery(DBPAGETABLE, $fields, null, null, $orderby);

		if(!isset($rs[2]))
		{
			while ($res = mysql_fetch_array($rs[0]))
			{
				$page = array(0 => $res['id'], 1 => $res['name'], 2 => $res['description'], 3 => $res['templatefile']);
				if (isset($pages[0]))
				{
					array_push($pages, $page);
				}
				else
				{
					$pages[0] = $page;
				}

				unset($page);
			}
		}
		unset($rs);
		unset($res);
		unset($DBA);
		unset($page);

		return($pages);
    }

    function getSections ()
    {
        $DBA = new DatabaseInterface();
        $sections = array();

		$fields = array(0 => 'id', 1 => 'name', 2 => 'description', 3 => 'templatefile', 4 => 'section_item');
        $orderby = 'id asc';

		$rs = $DBA->selectQuery(DBSECTIONSTABLE, $fields, null, null, $orderby);

		if(!isset($rs[2]))
		{
			while ($res = mysql_fetch_array($rs[0]))
			{
				$section = array(0 => $res['id'], 1 => $res['name'], 2 => $res['description'], 3 => $res['templatefile'], 4 => $res['section_item']);
				if (isset($sections[0]))
				{
					array_push($sections, $section);
				}
				else
				{
					$sections[0] = $section;
				}

				unset($section);
			}
		}
		unset($rs);
		unset($res);
		unset($DBA);
		unset($section);

		return($sections);
    }

    function getSectionItemsList ($sectionid)
    {
        $section_name = getSectionName($sectionid);
        $DBA = new DatabaseInterface();

        $fields = array(0 => 'max(item_id) as id');
        $idfields = array(0 => 'section_id');
        $idvals = array(0 => $sectionid);
        $orderby = 'id desc';

        $DBA = new DatabaseInterface();

        $rs = $DBA->selectQuery(DBPAGECONTENTTABLE, $fields, $idfields, $idvals, $orderby);
        if(!isset($rs[2]))
        {
            while($res = mysql_fetch_array($rs[0]))
            {
                 $idtoget = $res['id'];
            }
        }

        unset($rs);
        unset($res);
        unset($fields);
        unset($idfields);
        unset($idvals);
        unset($DBA);

        $DBA = new DatabaseInterface();

        if ($idtoget!=0)
        {
            do {
                $subitem = array();

                $fields = array(0 => 'inputname', 1 => 'inputvalue');
                $idfields = array(0 => 'item_id', 1 => 'section_id');

                $idvals = array(0 => $idtoget, 1 => $sectionid);
                $orderby = 'id';

                $DBA = new DatabaseInterface();

                $rs = $DBA->selectQuery(DBPAGECONTENTTABLE, $fields, $idfields, $idvals, $orderby);

                if ((!isset($rs[2])) && (!$rs[1]==0))
                {
                    $subitem['dbid'] = $idtoget;
                    $subitem['path'] = SITE_PATH . $section_name . '/' . $idtoget . '/';
                    while ($res = mysql_fetch_array($rs[0]))
                    {
                        $subitem[$res['inputname']] = html_entity_decode($res['inputvalue'],ENT_QUOTES,'ISO-8859-1');
                    }

                    if (isset($itemlist[0]))
                    {
                        array_push($itemlist, $subitem);
                    }
                    else
                    {
                        $itemlist[0] = $subitem;
                    }

                    $totid-=1;
                }

                $idtoget-=1;

                unset($rs);
                unset($res);
                unset($fields);
                unset($idfields);
                unset($idvals);
                unset($DBA);
            } while (!$idtoget==0);
        }

        unset($idtoget);
        unset($rs);
        unset($res);
        unset($fields);
        unset($idfields);
        unset($idvals);
        unset($DBA);

        return $itemlist;
    }

    function getItems ()
    {
        $DBA = new DatabaseInterface();
        $items = array();

		$fields = array(0 => 'id', 1 => 'name', 2 => 'description', 3 => 'templatefile', 4 => 'section_item', 5 => 'reltable', 6 => 'table_id_field', 7 => 'table_order_field');
        $orderby = 'id asc';

		$rs = $DBA->selectQuery(DBINDIVIDUALITEMSTABLE, $fields, null, null, $orderby);

		if(!isset($rs[2]))
		{
			while ($res = mysql_fetch_array($rs[0]))
			{
				$item = array(0 => $res['id'], 1 => $res['name'], 2 => $res['description'], 3 => $res['templatefile'], 4 => $res['section_item'], 5 => $res['reltable'], 6 => $res['table_id_field'], 7 => $res['table_order_field']);
				if (isset($items[0]))
				{
					array_push($items, $item);
				}
				else
				{
					$items[0] = $item;
				}

				unset($item);
			}
		}
		unset($rs);
		unset($res);
		unset($DBA);
		unset($item);

		return($items);
    }

    function getIndividualItemsList ($itemid,$tablename,$sortby)
    {
        $itemlist = array();
        $DBA = new DatabaseInterface();

        $fields = array(0 => '*');

        $rs = $DBA->selectQuery($tablename, $fields);

        if (!isset($rs[2]))
        {
            while ($res = mysql_fetch_array($rs[0]))
            {
                $subitem = $res;

                if ($sortby > 0)
                {
                    if (isset($itemlist[0]))
                    {
                        $found = false;
                        //we need to sort the item array by sortby
                        $counter = 0;
                        $tot = count($itemlist);
                        while (($counter <= $tot) && (!$found == true))
                        {
                            //check the value of $itemlist[$count][$sortby] against $item[$sortby]
                            if ($subitem[$sortby] > $itemlist[$counter][$sortby])
                            {
                                //higher check next and not found
                                $found = false;
                            }
                            else
                            {
                                //we have to put it in the previous position and arrange other keys
                                $found = true;
                                $temparr = array();
                                $temparr = array_slice($itemlist, $counter, $tot-$counter);
                                array_splice($itemlist, $counter, $tot-$counter, $subitem);
                                foreach ($temparr as $temp)
                                {
                                    array_push($itemlist,$temp);
                                }
                                array_push($itemlist, $temparr);
                                unset($temparr);
                            }
                            $counter+=1;
                        }

                        if ($found == false)
                        {
                            array_push($itemlist, $subitem);
                        }
                    }
                    else
                    {
                        array_push($itemlist, $subitem);
                    }
                }
                else
                {
                    array_push($itemlist, $subitem);
                }
            }
        }
        unset($idtoget);
        unset($rs);
        unset($res);
        unset($fields);
        unset($idfields);
        unset($idvals);
        unset($DBA);

        return $itemlist;
    }

    function getSectionItemsbyCat($sectionid, $catid, $fname)
    {
        //section id = constant from app.kfc passd in by calling
        //catid = the id of the category from the defined table
        //fname = the field name of the field in page content table
        //get section items then troll through looking for matching catid in fname, add to array and return
        $items = getSectionItemsList($sectionid);

        $returnlist = array();

        if (isset($items))
        {
            foreach($items as $item)
            {
                if ($item[$fname] == $catid)
                {
                    array_push($returnlist, $item);
                }
            }
        }

        unset($items);
        return $returnlist;
    }

    function getSectionName($sectionid)
    {
        $secfields = array(0 => 'name');
        $secidfields = array(0 => 'id');
        $secidvals = array(0 => $sectionid);

        $secDBA = new DatabaseInterface();

        $secrs = $secDBA->selectQuery(DBSECTIONSTABLE, $secfields, $secidfields, $secidvals);
        if (!isset($secrs[2]))
        {
            while ($secres = mysql_fetch_array($secrs[0]))
            {
                $section_name = $secres['name'];
            }
        }

        unset($secfields);
        unset($secidfields);
        unset($secidvals);
        unset($secrs);
        unset($secres);
        unset($secDBA);

        return $section_name;
    }

    function getSectionID($sectionname)
    {
        $DBA = new DatabaseInterface();
        $fields = array(0 => 'id');
        $idfields = array(0 => 'name');
        $idvals = array(0 =>$sectionname);

        $rs = $DBA->selectQuery(DBSECTIONSTABLE, $fields, $idfields, $idvals);

        if (!isset($rs[2]))
        {
            while ($res = mysql_fetch_array($rs[0]))
            {
                $sectionid = $res['id'];
            }
        }

        unset($DBA, $fields, $idfields, $idvals, $rs, $res);

        return ($sectionid);
    }

    function getSectionIDbyItem($itemtype)
    {
        $DBA = new DatabaseInterface();
        $fields = array(0 => 'id');
        $idfields = array(0 => 'section_item');
        $idvals = array(0 =>$itemtype);

        $rs = $DBA->selectQuery(DBSECTIONSTABLE, $fields, $idfields, $idvals);

        if (!isset($rs[2]))
        {
            while ($res = mysql_fetch_array($rs[0]))
            {
                $sectionid = $res['id'];
            }
        }

        unset($DBA, $fields, $idfields, $idvals, $rs, $res);

        return ($sectionid);
    }

    function getDataItemIDbyItem($itemtype)
    {
        $DBA = new DatabaseInterface();

        $fields = array(0 => 'id');
        $idfields = array(0 => 'section_item');
        $idvals = array(0 => $itemtype);

        $rs = $DBA->selectQuery(DBINDIVIDUALITEMSTABLE, $fields, $idfields, $idvals);

        if (!isset($rs[2]))
        {
            while ($res = mysql_fetch_array($rs[0]))
            {
                $sectionid = $res['id'];
            }
        }

        unset($DBA, $fields, $idfields, $idvals, $rs, $res);

        return ($sectionid);
    }

    function sectionoritem($itemtype)
    {
        $id = getSectionIDbyItem($itemtype);

        if (isset($id))
        {
            $type = 'S';
            return $type;
        }
        else
        {
            $id = getDataItemIDbyItem($itemtype);

            if (isset($id))
            {
                $type = 'I';
                return $type;
            }
        }

        $type = 'X';
        return $type;
    }

    function getLatest($sectionid, $numtofetch=1)
    {
        $secfields = array(0 => 'name');
        $secidfields = array(0 => 'id');
        $secidvals = array(0 => $sectionid);

        $secDBA = new DatabaseInterface();

        $secrs = $secDBA->selectQuery(DBSECTIONSTABLE, $secfields, $secidfields, $secidvals);
        if (!isset($secrs[2]))
        {
            while ($secres = mysql_fetch_array($secrs[0]))
            {
                $section_name = $secres['name'];
            }
        }

        unset($secfields);
        unset($secidfields);
        unset($secidvals);
        unset($secrs);
        unset($secres);
        unset($secDBA);

        $fields = array(0 => 'max(item_id) as id');
        $idfields = array(0 => 'section_id');
        $idvals = array(0 => $sectionid);

        $DBA = new DatabaseInterface();

        $rs = $DBA->selectQuery(DBPAGECONTENTTABLE, $fields, $idfields, $idvals);
        if(!isset($rs[2]))
        {
            while($res = mysql_fetch_array($rs[0]))
            {
                 $idtoget = $res['id'];
            }
        }

        unset($rs);
        unset($res);
        unset($fields);
        unset($idfields);
        unset($idvals);
        unset($DBA);

        $totid = $numtofetch;

        do {
            $item = array();

            $fields = array(0 => 'inputname', 1 => 'inputvalue');
            $idfields = array(0 => 'item_id', 1 => 'section_id');
            $idvals = array(0 => $idtoget, 1 => $sectionid);

            $DBA = new DatabaseInterface();

            $rs = $DBA->selectQuery(DBPAGECONTENTTABLE, $fields, $idfields, $idvals);

            if ((!isset($rs[2])) && (!$rs[1]==0))
            {
                $item['path'] = SITE_PATH . $section_name . '/' . $idtoget . '/';
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $item[$res['inputname']] = html_entity_decode($res['inputvalue'],ENT_QUOTES,'ISO-8859-1');
                }

                if (isset($itemlist[0]))
                {
                    array_push($itemlist, $item);
                }
                else
                {
                    $itemlist[0] = $item;
                }

                $totid-=1;
            }

            $idtoget-=1;

            unset($rs);
            unset($res);
            unset($fields);
            unset($idfields);
            unset($idvals);
            unset($DBA);
        } while (($totid >= 1) && (!$idtoget==0));

        unset($idtoget);
        unset($rs);
        unset($res);
        unset($fields);
        unset($idfields);
        unset($idvals);
        unset($DBA);
        unset($section_name);

        return $itemlist;
    }

    function getItemTypes($userauthlevel)
    {
        if ($userauthlevel == 0)
        {
            //superadmin
            $return[0] = array('group' => 'Administrator');
            $return[1] = array('name' => 'Page');
            $return[2] = array('name' => 'Section');
            $return[3] = array('name' => 'Data Item');
            if (NEWSLETTERS_ACTIVE == 1)
            {
                $return[4] = array('name' => 'Newsletter Template');
            }
        }

        if ($userauthlevel <=1 )
        {
            //admin
            if (NEWSLETTERS_ACTIVE == 1)
            {
                $counter = 5;
            }
            else
            {
                $counter = 4;
            }
            $return[$counter] = array('group' => 'Items');
            $DBA = new DatabaseInterface();

            $fields = array(0 => 'section_item');
            $orderby = 'id desc';

            $rs = $DBA->selectQuery(DBSECTIONSTABLE, $fields, null, null, $orderby);

            if (!isset($rs[2]))
            {
                if (NEWSLETTERS_ACTIVE == 1)
                {
                    $counter=6;
                }
                else
                {
                    $counter = 5;
                }

                while ($res = mysql_fetch_array($rs[0]))
                {
                    $return[$counter] = array('name' => $res['section_item']);
                    $counter += 1;
                }
            }

            unset($fields, $orderby, $rs, $DBA, $res);

            $fields = array(0 => 'section_item');
            $orderby = 'id desc';

            $DBA = new DatabaseInterface();

            $rs = $DBA->selectQuery(DBINDIVIDUALITEMSTABLE, $fields, null, null, $orderby);

            if (!isset($rs[2]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    $return[$counter] = array('name' => $res['section_item']);
                    $counter += 1;
                }
            }

            if (NEWSLETTERS_ACTIVE == 1)
            {
                $return[$counter] = array('group' => 'Newletters');
                $counter += 1;
                $return[$counter] = array('name' => 'Mailing List');
                $counter += 1;
                $fields = array(0 => 'name');
                $DBA = new DatabaseInterface();
                $rs = $DBA->selectQuery(DBNEWSLETTERTEMPLATETABLE, $fields);

                if (!isset($rs[2]))
                {
                    while ($res = mysql_fetch_array($rs[0]))
                    {
                        $return[$counter] = array('name' => $res['name']);
                        $counter += 1;
                    }
                }
            }
        }
        return $return;
    }
?>