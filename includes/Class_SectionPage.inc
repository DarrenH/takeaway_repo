<?php
	    //********************************************************************//
    //   Application : Kooshtie Framework Layout                          //
    //   Filename : Class_Page.inc                           //
    //                                                                    //
    //   Author : Darren Hiles                                            //
    //   ©Kooshtie 2008 All Rights Reserved                               //
    //********************************************************************//

    class sectionPage extends Page {
		var $SectionID,
            $SectionItem,
            $sTags,
            $Items,
            $MultiPage;


		function sectionPage()
		{

		}

        function getSectionPage($ID)
        {
			$DBA = new DatabaseInterface();

			$fields = array(0 => 'name', 1 => 'description', 2 => 'templatefile', 3 => 'dispmm', 4 => 'seclevel', 5 => 'section_item');
			$idfields = array(0 => 'id');
			$idvals = array(0 => $ID);

			$rs = $DBA->selectQuery(DBSECTIONSTABLE, $fields, $idfields, $idvals);
			if (!isset($rs[2]))
			{
				while ($res = mysql_fetch_array($rs[0]))
				{
					$this->setPageVar('SectionID', $ID);
					$this->setPageVar('Name', $res['name']);
					$this->setPageVar('Description', $res['description']);
					$this->setPageVar('Templatefile', $res['templatefile']);
                    $this->setPageVar('Dispmm', $res['dispmm']);
                    $this->checkMenuPos();
                    $this->setPageVar('Securitylevel', $res['seclevel']);
                    $this->setPageVar('SectionItem', $res['section_item']);
				}
			}
        }

        function checkMenuPos()
        {
            if ($this->Dispmm == 1)
            {
                $DBA = new DatabaseInterface();
                $fields = array(0 => 'mmorder');
                $idfields = array(0 => 'id', 1 => 'name', 2 => 'type');
                $idvals = array(0 => $this->SectionID, 1 => $this->Name, 2 => 's');

                $rs = $DBA->selectQuery(DBMENUTABLE, $fields, $idfields, $idvals);
                if (!isset($rs[2]))
                {
                    while ($res = mysql_fetch_array($rs[0]))
                    {
                        $this->setPageVar('Menupos', $res['mmorder']);
                    }
                }
                unset($res);
                unset($rs);
                unset($fields);
                unset($idfields);
                unset($idvals);
                unset($DBA);
            }
            else
            {
                $this->setPageVar('Menupos','');
            }
        }
        function getsectionTemplate()
        {
			if (isset($this->Templatefile))
			{
				$this->setPageVar('Templatelocation', SERVER_ROOT.TPLFOLDER.$this->Templatefile);
			}
        }

        function getsectionTemplateTags()
        {
            if(!isset($this->SectionID))
            {
                unset($this->Tags);
            }
            else
            {
                $sectionopen = 0;
                $tpl = fopen($this->Templatelocation, 'r');
    			while (!feof($tpl))
    			{
    				//grab a line of text from the file
    				$searchstr = fgets($tpl);

    				//does it contain a tag?
    				if (strpos($searchstr, '<tag'))
    				{
    					$searchstr = trim($searchstr);

    					//we have found a tag now we need to use it to get the name/description/type of the item
    					$tag = explode(' ', $searchstr);
    					//$tag now contains somthing like
    					//$tag[0] = '<tag'
    					//$tag[1] = 'name="title"'
    					//$tag[2] = 'description="Text which is displayed at the top of the browser window"'
    					//$tag[3] = 'type="inputbox'
    					//$tag[4] = ' />';

                        if ($sectionopen == 0)
                        {
        					$inputlist['name'] = $this->formTags($tag[1]); //name
        					$inputlist['description'] = $this->formTags($tag[2]); //description
    	    				$inputlist['type'] = $this->formTags($tag[3]); //type

                            if (($inputlist['name'] == 'keywords') || ($inputlist['name'] == 'description'))
                            {
                                if (isset($this->SEOTags[0]))
                                {
                                    array_push($this->SEOTags, $inputlist);
                                }
                                else
                                {
                                    $this->SEOTags[0] = $inputlist;
                                }
                            }
                            else
                            {
                                if (isset($this->Tags[0]))
            					{
            						array_push($this->Tags, $inputlist);
        	    				}
        		    			else
        			    		{
        				    		$this->Tags[0] = $inputlist;
        					    }
                            }
                        }
                        else
                        {
        					$sectioninputlist['name'] = $this->formTags($tag[1]); //name
        					$sectioninputlist['description'] = $this->formTags($tag[2]); //description
    	    				$sectioninputlist['type'] = $this->formTags($tag[3]); //type
                            if ((isset($tag[4])) && ($tag[4]!=' />'))
                            {
                                $sectioninputlist['table'] = $this->formTags($tag[4]);
                            }

                            if (isset($this->sTags[0]))
        					{
        						array_push($this->sTags, $sectioninputlist);
    	    				}
    		    			else
    			    		{
    				    		$this->sTags[0] = $sectioninputlist;
    					    }
                        }
                        unset($this->Inputlist);
    				}

                    if (strpos($searchstr, '<item'))
                    {
                        $sectionopen = 1;
                    }
                    elseif (strpos($searchstr, '</item'))
                    {
                        $sectionopen = 0;
                    }
    			}

    			fclose($tpl);
            }
        }

        function getsectionTemplatensTags()
        {
            if(!isset($this->SectionID))
            {
                unset($this->Tags);
            }
            else
            {
                $sectionopen = 0;
                $tpl = fopen($this->Templatelocation, 'r');
    			while (!feof($tpl))
    			{
    				//grab a line of text from the file
    				$searchstr = fgets($tpl);

    				//does it contain a tag?
    				if (strpos($searchstr, '<tag'))
    				{
    					$searchstr = trim($searchstr);

    					//we have found a tag now we need to use it to get the name/description/type of the item
    					$tag = explode(' ', $searchstr);
    					//$tag now contains somthing like
    					//$tag[0] = '<tag'
    					//$tag[1] = 'name="title"'
    					//$tag[2] = 'description="Text which is displayed at the top of the browser window"'
    					//$tag[3] = 'type="inputbox'
    					//$tag[4] = ' />';

                        if ($sectionopen == 0)
                        {
        					$inputlist['name'] = $this->formTags($tag[1]); //name
        					$inputlist['description'] = $this->formTags($tag[2]); //description
    	    				$inputlist['type'] = $this->formTags($tag[3]); //type
                            if (isset($this->Tags[0]))
          					{
          						array_push($this->Tags, $inputlist);
      	    				}
      		    			else
      			    		{
      				    		$this->Tags[0] = $inputlist;
      					    }
                        }
                        else
                        {
        					$sectioninputlist['name'] = $this->formTags($tag[1]); //name
        					$sectioninputlist['description'] = $this->formTags($tag[2]); //description
    	    				$sectioninputlist['type'] = $this->formTags($tag[3]); //type
                            if ((isset($tag[4])) && ($tag[4]!=' />'))
                            {
                                $sectioninputlist['table'] = $this->formTags($tag[4]);
                            }

                            if (isset($this->sTags[0]))
        					{
        						array_push($this->sTags, $sectioninputlist);
    	    				}
    		    			else
    			    		{
    				    		$this->sTags[0] = $sectioninputlist;
    					    }
                        }
                        unset($this->Inputlist);
    				}

                    if (strpos($searchstr, '<item'))
                    {
                        $sectionopen = 1;
                    }
                    elseif (strpos($searchstr, '</item'))
                    {
                        $sectionopen = 0;
                    }
    			}

    			fclose($tpl);
            }
        }

        function getItems()
        {
            unset($items);
            $items = array();
            //uses $this->SectionID to get the individual number of items in the db
            $fields = array(0 => 'distinct(item_id) as item_id');
            $idfields = array(0 => 'section_id', 1 => 'item_id');
            $idvals = array(0 => $this->SectionID, 1 =>'!0');
            $orderby = 'item_id desc';

            $DBA = new DatabaseInterface();
            $rs = $DBA->selectQuery(DBPAGECONTENTTABLE, $fields, $idfields, $idvals, $orderby);

            if ((!isset($rs[2])) && ($rs[1] >=1))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    if (isset($items[0]))
                    {
                        array_push($items, $res['item_id']);
                    }
                    else
                    {
                        $items[0] = $res['item_id'];
                    }
                }
            }

            unset($res);
            unset($rs);
            unset($idvals);
            unset($idfields);
            unset($fields);
            unset($DBA);

            $this->setPageVar('Items', $items);
        }

        function formscommonInput($inputarray)
        {
            $labelstr = '<label for="' . $inputarray['name'] . '">' . str_replace('_', ' ', $inputarray['description']) . '</label>';
  			//search db with $this->ID and $inputarray['name'] to get the value of the box
  			$DBA = new DatabaseInterface();

  			$fields = array(0 => 'inputvalue');
  			$idfields = array(0 => 'pageid', 1 => 'inputname', 2 => 'section_id', 3 => 'item_id');
  			$idvals = array(0 => '0', 1 => $inputarray['name'], 2 => $this->SectionID, 3 => '0');

  			$rs = $DBA->selectQuery(DBPAGECONTENTTABLE, $fields, $idfields, $idvals);

  			if (!isset($rs[2]))
  			{
  				while ($res = mysql_fetch_array($rs[0]))
  				{
  					$value = html_entity_decode($res['inputvalue'], ENT_QUOTES, 'ISO-8859-1');
  				}
  			}

			//now make the input
			switch($inputarray['type'])
			{
                case 'inputbox':
					//<input type="text">
					$retstr .= $labelstr . '<input type="text" name="' . $inputarray['name'] . '" value="' . $value . '" />';
					break;
				case 'textarea':
					//<textarea>
					$retstr .= $labelstr . '<textarea name="' . $inputarray['name'] . '">' . $value . '</textarea>';
					break;
				case 'content':
					//fck
					$retstr = 'fck';
					break;
				case 'check':
					//<input type="checkbox">
					$retstr .= $labelstr . '<input type="checkbox" name="' . $inputarray['name'] . '" value="checked" class="check" ' . ($value=="checked" ? 'checked="checked" ' : '') . '/>';
					break;
                case 'filebox':
                    $retstr .= '<fieldset class="file">';
                    if (isset($value))
                    {
                        $DBA = new DatabaseInterface();

                        $fields = array(0 => 'filename');
                        $idfields = array(0 => 'id');
                        $idvals = array(0 => $value);

                        $rs = $DBA->selectQuery(DBFILETABLE, $fields, $idfields, $idvals);

                        if ((!isset($rs[2])) && (!$rs[1]==0))
                        {
                            while ($res = mysql_fetch_array($rs[0]))
                            {
                                $retstr .= '<span class="filename">Current filename: ' . $res['filename'] . '<span class="delfile"><a href="' . SITE_PATH . ADMIN_FOLDER . 'delete_file/" title="Delete this file"><img src="' . SITE_PATH . ADMIN_FOLDER . 'images/delete-page.png" alt="Click here to delete this page" /><span>Delete this file</span></a></span></span>';
                                $retstr .= '<p>To Change this file please first delete the old version.</p>';
                                $retstr .= '<input type="hidden" class="offscreen" name="' . $inputarray['name'] . '" value="' . $res['filename'] . '" />';
                            }
                        }
                        else
                        {
                            $retstr .= '<input type="file" name="' . $inputarray['name'] . '" id="' . $inputarray['name'] . '" class="nobg" />';
                        }

                        $retstr = $labelstr . $retstr;

                        unset($fields);
                        unset($idfields);
                        unset($idvals);
                        unset($DBA);
                    }
                    $retstr .= '</fieldset>';
                    break;
				case 'radio':
					//<input type="radio">
					$reststr .= $labelstr . '<input type="radio" name="' . $inputarray['name'] . '" value="' . $value . '" />';
					break;
				case 'drop':
					//<select>
                    $sDBA = new DatabaseInterface();
                    $sfields = array(0 => 'id', 1 => 'name');

                    $sDBA = new DatabaseInterface();
                    $srs = $sDBA->selectQuery($inputarray['table'], $sfields);
                    if(!isset($srs[2]))
                    {
                        $retstr .= $labelstr . '<select name="' . $inputarray['name'] . '" id="' . $inputarray['name'] . '">';
                        while($sres = mysql_fetch_array($srs[0]))
                        {
                            $retstr .= '<option value="' . $sres['id'] . '"';
                            if ($sres['name'] == $inputarray['value'])
                            {
                                $retstr .= ' selected="selected"';
                            }
                            $retstr .= ' >' . $sres['name'] . '</option>';
                        }
                        $retstr .= '</select>';
                    }
                    unset($sfields);
                    unset($srs);
                    unset($sDBA);
					break;
				default:
					//<input type="text">
					$retstr .= $labelstr . '<input type="text" name="' . $inputarray['name'] . '" value="' . $res['value'] . '" />';
					break;
			}

			return $retstr;
        }

        function formsInput($inputarray, $itemid=null)
		{
			$labelstr = '<label for="' . $inputarray['name'] . '">' . str_replace('_', ' ', $inputarray['description']) . '</label>';
            if (!is_null($itemid))
            {
    			//search db with $this->ID and $inputarray['name'] to get the value of the box
    			$DBA = new DatabaseInterface();

    			$fields = array(0 => 'inputvalue');
    			$idfields = array(0 => 'pageid', 1 => 'inputname', 2 => 'section_id', 3 => 'item_id');
    			$idvals = array(0 => '0', 1 => $inputarray['name'], 2 => $this->SectionID, 3 => $itemid);

    			$rs = $DBA->selectQuery(DBPAGECONTENTTABLE, $fields, $idfields, $idvals);

    			if (!isset($rs[2]))
    			{
    				while ($res = mysql_fetch_array($rs[0]))
    				{
    					$value = html_entity_decode($res['inputvalue'], ENT_QUOTES, 'ISO-8859-1');
    				}
    			}
            }
            else
            {
                unset($value);
            }

			//now make the input
			switch($inputarray['type'])
			{
                case 'inputbox':
					//<input type="text">
					$retstr .= $labelstr . '<input type="text" name="' . $inputarray['name'] . '" value="' . $value . '" />';
					break;
				case 'textarea':
					//<textarea>
					$retstr .= $labelstr . '<textarea name="' . $inputarray['name'] . '">' . $value . '</textarea>';
					break;
				case 'content':
					//fck
					$retstr = 'fck';
					break;
				case 'check':
					//<input type="checkbox">
					$retstr .= $labelstr . '<input type="checkbox" name="' . $inputarray['name'] . '" value="checked" class="check" ' . ($value=="checked" ? 'checked="checked" ' : '') . '/>';
					break;
                case 'filebox':
                    $retstr .= '<fieldset class="file"><span class="legend">File Upload</span>' . $labelstr;
                    if (isset($value))
                    {
                        $DBA = new DatabaseInterface();

                        $fields = array(0 => 'filename');
                        $idfields = array(0 => 'id');
                        $idvals = array(0 => $value);

                        $rs = $DBA->selectQuery(DBFILETABLE, $fields, $idfields, $idvals);
                        if ((!isset($rs[2])) && (!$rs[1]==0))
                        {
                            while ($res = mysql_fetch_array($rs[0]))
                            {
                                $retstr .= '<span class="filename">Current filename: ' . $res['filename'] . '<span class="delfile"><a href="' . SITE_PATH . ADMIN_FOLDER . 'delete_file/" title="Delete this file"><img src="' . SITE_PATH . ADMIN_FOLDER . 'images/delete-page.png" alt="Click here to delete this page" /><span>Delete this file</span></a></span></span>';
                                $retstr .= '<p>To Change this file please first delete the old version.</p>';
                                $retstr .= '<input type="hidden" class="hidden" name="' . $inputarray['name'] . '" value="' . $value . '" />';
                            }
                        }

                        $retstr = $labelstr . $retstr;

                        unset($fields);
                        unset($idfields);
                        unset($idvals);
                        unset($DBA);
                    }
                    else
                    {
                        $retstr .= '<input type="file" name="' . $inputarray['name'] . '" id="' . $inputarray['name'] . '" class="nobg" />';
                    }
                    $retstr .= '</fieldset>';
                    break;
				case 'radio':
					//<input type="radio">
					$reststr .= $labelstr . '<input type="radio" name="' . $inputarray['name'] . '" value="' . $value . '" />';
					break;
				case 'drop':
					//<select>
                    $sDBA = new DatabaseInterface();
                    $sfields = array(0 => 'id', 1 => 'name');

                    $sDBA = new DatabaseInterface();
                    $srs = $sDBA->selectQuery($inputarray['table'], $sfields);
                    if(!isset($srs[2]))
                    {
                        $retstr .= $labelstr . '<select name="' . $inputarray['name'] . '" id="' . $inputarray['name'] . '">';
                        while($sres = mysql_fetch_array($srs[0]))
                        {
                            $retstr .= '<option value="' . $sres['id'] . '"';
                            if ($sres['id'] == $value)
                            {
                                $retstr .= ' selected="selected"';
                            }
                            $retstr .= ' >' . $sres['name'] . '</option>';
                        }
                        $retstr .= '</select>';
                    }
                    unset($sfields);
                    unset($srs);
                    unset($sDBA);
					break;
				default:
					//<input type="text">
					$retstr .= $labelstr . '<input type="text" name="' . $inputarray['name'] . '" value="' . $res['value'] . '" />';
					break;
			}

			return $retstr;
		}

        function deleteMenu()
        {
            $sql = "delete from " . DBMENUTABLE . " where id = " . $this->SectionID . " and name = '" . $this->Name . "' and type = 's'";

            $DBA = new DatabaseInterface();

            $rs = $DBA->runQuery($sql);

  		    unset($sql);
      		unset($DBA);

            if (isset($rs[2]))
            {
                return(1);
            }
            else
            {
                return(0);
            }
        }

        function saveMenu()
        {
            unset($menuerror);
            $fields = array(0 => 'id', 1 => 'name', 2 => 'type', 3 => 'mmorder');
            $fieldvals = array(0 => $this->SectionID, 1 => str_replace(' ', '_',$this->Name), 2 => 's', 3 => $this->Menupos);
            $DBA = new DatabaseInterface();
            $rs = $DBA->insertQuery(DBMENUTABLE, $fields, $fieldvals);
            if (isset($rs2[2]))
            {
               $menuerror = 1;
            }
            unset($rs);
      		unset($fields);
         	unset($fieldvals);
      		unset($DBA);
            if (isset($menuerror))
            {
                return (2);
            }
            else
            {
                return (0);
            }
        }

        function saveSection()
        {
            unset($error);
            $DBA = new DatabaseInterface();
            $fields = array(0 => 'name', 1 => 'description', 2 => 'templatefile', 3 => 'seclevel', 4 => 'dispmm', 5 => 'section_item');
  		    $fieldvals = array(0 => str_replace(' ', '_',$this->Name), 1 => $this->Description, 2 => $this->Templatefile, 3 => $this->Securitylevel, 4 => $this->Dispmm, 5 => $this->SectionItem);
            $idfield = 'id';
            $idval = $this->SectionID;
            $rs = $DBA->updateQuery(DBSECTIONSTABLE, $fields, $fieldvals, $idfield, $idval);

      		if ($rs == 1)
      		{
                if ($this->Dispmm == '1')
                {
                    //save a menu record
                    $error = $this->saveMenu();
                }
      		}
      		else
      		{
                $error = 1;
      		}

      		unset($rs,$res,$fields,$fieldvals,$idfield,$idval,$DBA);

            return ($error);
        }

        function deletePageContent()
        {
            $DBA = new DatabaseInterface();
            $idfield = 'section_id';
            $idval = $this->SectionID;
            $rs = $DBA->deleteRecord(DBPAGECONTENTTABLE, $idfield, $idval);
            if (isset($rs[2]))
            {
                return(1);
            }
            else
            {
                return(0);
            }
        }

        function deletePageRecord()
        {
            $DBA = new DatabaseInterface();
            $idfield = 'id';
            $idval = $this->SectionID;
            $rs = $DBA->deleteRecord(DBSECTIONSTABLE, $idfield, $idval);
            if (isset($rs[2]))
            {
                return(1);
            }
            else
            {
                return(0);
            }
        }

        function deletePage()
        {
            unset($error);
            $res1 = $this->deleteMenu();
            $res2 = $this->deletePageContent();
            $res3 = $this->deletePageRecord();
            if ($res1 == 1)
            {
                error_push('Error deleting menu record.');
                $error = 1;
            }
            if ($res2 == 1)
            {
                error_push('Error deleting section content');
                $error = 1;
            }
            if($res3 == 1)
            {
                error_push('Error deleting section record');
                $error = 1;
            }
            return($error);
        }
	}
?>
