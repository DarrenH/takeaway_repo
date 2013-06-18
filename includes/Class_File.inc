<?php
    //********************************************************************//
    //   Application : Kooshtie Framework Layout                          //
    //   Filename : Class_DatabaseInterface.inc                           //
    //                                                                    //
    //   Author : Darren Hiles                                            //
    //   ©Kooshtie 2008 All Rights Reserved                               //
    //********************************************************************//

    class File {
        var $ID,
        $Date_Uploaded,
        $FileName,
        $FileFolderID,
        $FileFolderName,
        $SysFileType,
        $FileType,
        $Owner_ID,
        $Owner_Name,
		$Resize,
        $Hidden;

        function File () {

        }

        function setFileVar($varname,$varvalue)
        {
            $this->$varname=$varvalue;
        }

        function getFile ($id=null)
        {
            //gets information about the file from the File table
            if (isset($id))
            {
                $fields = array(0 => 'filename', 1 => 'filefolderid', 2 => 'owner_id', 3 => 'date_uploaded', 4 => 'filetype');
                $idfields = array(0 => 'id');
                $idvals = array(0 => $id);

                $DBA = new DatabaseInterface();

                $rs = $DBA->selectQuery(DBFILETABLE, $fields, $idfields, $idvals);

                if (!isset($rs[2]))
                {
                    while ($res = mysql_fetch_array($rs[0]))
                    {
                        $this->setFileVar('ID', $id);
                        $this->setFileVar('FileName', $res['filename']);
                        $this->setFileVar('FileFolderID', $res['filefolderid']);
                        $this->getFileLocation();
                        $this->setFileVar('Owner_ID', $res['owner_id']);
                        $this->getOwnerInfo();
                        $this->setFileVar('Date_Uploaded', $res['date_uploaded']);
                        $this->setFileVar('SysFileType', $res['filetype']);
                        $this->getFileType();
                    }
                }
                else
                {
                    session_push('Unable to find file!');
                }

                unset($rs);
                unset($res);
                unset($DBA);
            }
        }

        function createNew ()
        {
            //creates a new record in the database for the file
            $DBA = new DatabaseInterface();

            $fields = array(0 => 'id', 1 => 'filename', 2 => 'filefolderid', 3 => 'owner_id', 4 => 'filetype');
            $fieldvals = array(0 => $this->ID, 1 => $this->FileName, 2 => $this->FileFolderID, 3 => $this->Owner_ID, 4 => $this->SysFileType);

            $rs = $DBA->insertQuery(DBFILETABLE, $fields, $fieldvals);

            if (!isset($rs))
            {
                session_push('There was an error creating the record for this file. Please contact your support');
            }
            
            unset($rs);
            unset($DBA);
        }

        function getNewFileID()
        {
            //retrieves latest file id from the database
            $DBA = new DatabaseInterface();

            $fields = array(0 => 'id');
            $rs = $DBA->selectQuery(DBFILEIDTABLE, $fields);

            if (isset($rs[0]))
            {
                while ($res = mysql_fetch_array($rs[0]))
                {
                    if ($res['id']==0)
                    {
                        $fileid = 1;
                    }
                    else
                    {
                        $fileid = $res['id'];
                    }
                }
                $DBA2 = new DatabaseInterface();
                $sql = "update " . DBFILEIDTABLE . " set id = id+1";
                $DBA2->runQuery($sql);
                unset($DBA2);
            }
            else
            {
                $fileid = 1;
            }

            unset ($fields);
            unset ($rs);
            unset ($DBA);

            return $fileid;
        }

        function getFileLocation ()
        {
            //uses the file id to get the folder and information pertaining
            $fields2 = array(0 => 'location');
            $idfields2 = array(0 => 'id');
            $idvals2 = array(0 => $this->FileFolderID);

            $DBA2 = new DatabaseInterface();

            $rs2 = $DBA2->selectQuery(DBFILEFOLDERSTABLE, $fields2, $idfields2, $idvals2);
            if (isset($rs2[0]))
            {
                while ($res2 = mysql_fetch_array($rs2[0]))
                {
                    $this->setFileVar('FileFolderName', $res2['location']);
                }
            }

            unset($rs2);
            unset($res2);
            unset($DBA2);
        }

        function getFileType ()
        {
            //takes the file type information from the database and turns into type
            switch ($this->SysFileType)
            {
                case 'TXT':
                    $type = 'text/plain';
                    break;
				case 'JPG':
					$type = 'image/jpeg';
					break;
                case 'ZIP':
                    $type = 'multipart/x-zip';
                    break;
                case 'DOC':
                    $type = 'application/msword';
                    break;
                case 'PDF':
                    $type = 'application/pdf';
                    break;
                default :
                    $type = 'image/jpeg';
                    break;
            }

            $this->setFileVar('FileType', $type);
        }

        function getType ($filestr)
        {
            //finds the uploaded files type and updates object
            switch ($filestr)
            {
                case ('text/plain'):
                    $type = 'TXT';
                    break;
				case ('image/jpeg'):
					$type = 'JPG';
					break;
                case ('multipart/x-zip'):
                    $type = 'ZIP';
                    break;
                case ('application/msword'):
                    $type = 'DOC';
                    break;
                case ('application/pdf'):
                    $type = 'PDF';
                    break;
                default:
                    $type = 'JPG';
                    break;
            }

            $this->setFileVar('SysFileType', $type);
        }

        function getOwnerInfo ()
        {
            //Uses the owner id to get info from the user table
            $DBA2 = new DatabaseInterface();

            $fields2 = array(0 => 'username');
            $idfields2 = array(0 => 'id');
            $idvals2 = array(0 => $this->Owner_ID);

            $rs2 = $DBA2->selectQuery(DBUSERTABLE, $fields2, $idfields2, $idvals2);

            if (!isset($rs2[2]))
            {
                while ($res2 = mysql_fetch_array($rs2[0]))
                {
                    $this->setFileVar('Owner_Name', $res2['username']);
                }
            }

            unset($rs2);
            unset($res2);
            unset($DBA2);
        }

        function uploadFile ($kfiles)
        {
            //creates new file and updates location                       */
            /* $kfiles should hold a reproduced version of $_FILES array  */
            /* The references for this array are                          */
            /* 0 = name                                                   */
            /* 1 = type                                                   */
            /* 2 = tmpname                                                */
            /* 3 = error                                                  */
            /* 4 = size                                                   */
            $file_parts = pathinfo($kfiles[0]);

            $this->getFileLocation();
            $this->setFileVar('FileName', $file_parts['filename'] . rand() . '.' . $file_parts['extension']);
//            $this->setFileVar('FileName', basename($kfiles[0]));
            $this->setFileVar('Owner_ID', $_SESSION['user']['id']);
            $this->setFileVar('FileType', $kfiles[1]);
            $this->setFileVar('Date_Uploaded', $_SERVER['REQUEST_TIME']);
            $this->setFileVar('Hidden', 0);

            $this->getType($kfiles[1]);

            if(move_uploaded_file($kfiles[2], SERVER_ROOT . $this->FileFolderName . $this->FileName))
            {
                if ($this->SysFileType == 'JPG')
                {
                    $img = imagecreatefromjpeg(SERVER_ROOT . $this->FileFolderName . $this->FileName);
                    $cwidth = imagesx($img);
                    $cheight = imagesy($img);

                    if (($cwidth > MAXIMGWIDTH) || ($cheight > MAXIMGHEIGHT))
                    {
                        //image needs resized
                        if ($cwidth > $cheight)
                        {
                            $newwidth = MAXIMGWIDTH;
                            $ratio = MAXIMGWIDTH/$cwidth;
                            $newheight = $cheight * $ratio;
                            //$exceeded = 'maximum width';
                        }
                        else
                        {
                            $newheight = MAXIMGHEIGHT;
                            $ratio = MAXIMGHEIGHT/$cheight;
                            $newwidth = $cwidth * $ratio;
                            //$exceeded = 'maximum height';
                        }
                        $finalimg = imagecreatetruecolor($newwidth, $newheight);
                        imagecopyresized($finalimg,$img,0,0,0,0,$newwidth,$newheight,$cwidth,$cheight);
                        imagedestroy($img);
                        imagejpeg($finalimg, SERVER_ROOT . $this->FileFolderName . $this->FileName, 100);
                        imagedestroy($finalimg);
                        //session_push('The file: ' .$this->Filename . ' exceeded the ' . $exceeded . ' for the site and has been resized accordingly');
                    }
                }

                session_push('The file: ' . $this->FileName . ' has been uploaded successfully to the folder: ' . $this->FileFolderName);

                $this->createNew();
                $result = 1;


            }
            else
            {
                session_push('The file upload for ' . $this->FileName . ' was unsuccessful. Please try again. If the problem continues please contact your support.');
                $result = 0;
            }
			
			/*if (isset($this->Resize))
			{
				$this->resizeImage($this->Resize);
			} */

            return $result;
        }

        function createFileLink($linktype, $pageid, $section_id, $item_id)
        {
            $DBA = new DatabaseInterface();

            $fields = array(0 => 'file_id', 1 => 'link_type', 2 => 'file_type', 3 => 'pageid', 4 => 'section_id', 5 => 'item_id');
            $fieldvals = array(0 => $this->ID, 1 => $linktype, 2 => $file_type, 3 => $pageid, 4 => $section_id, 5 => $item_id);

            $rs = $DBA->insertQuery(DBFILELINKTABLE, $fields, $fieldvals);
        }

		function resizeImage($newwidth, $newheight, $newlocation=null)
		{
            $img = imagecreatefromjpeg(SERVER_ROOT . $this->FileFolderName . $this->FileName);
            $cwidth = imagesx($img);
            $cheight = imagesy($img);

            if (($cwidth > $newwidth) || ($cheight > $newheight))
            {
                //image needs resized
                if ($cwidth > $cheight)
                {
                    $newwidth = $newwidth;
                    $ratio = $newwidth/$cwidth;
                    $newheight = $cheight * $ratio;
                }
                else
                {
                    $newheight = $newhieght;
                    $ratio = $newheight/$cheight;
                    $newwidth = $cwidth * $ratio;
                }
                $finalimg = imagecreatetruecolor($newwidth, $newheight);
                imagecopyresized($finalimg,$img,0,0,0,0,$newwidth,$newheight,$cwidth,$cheight);
                imagedestroy($img);
                if (isset($newlocation))
                {
                    $location = SERVER_ROOT . $newlocation . $this->FileName;
                }
                else
                {
                    $location = SERVER_ROOT . $this->FileFolderName . $this->FileName;
                }
                imagejpeg($finalimg, SERVER_ROOT . $this->FileFolderName . $this->FileName, 100);
                imagedestroy($finalimg);
            }
		}

        function deleteFile ($id)
        {
            //removes info for the file and the file from the server?

        }

        function hideFile ()
        {
            //disable the ability to view the file except to admin
        }
    }
?>
