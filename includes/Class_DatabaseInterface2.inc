<?php
    //********************************************************************//
    //   Application : Kooshtie Framework Layout                          //
    //   Filename : Class_DatabaseInterface.inc                           //
    //                                                                    //
    //   Author : Darren Hiles                                            //
    //   �Kooshtie 2008 All Rights Reserved                               //
    //********************************************************************//


    class DatabaseInterface {
        var $DBHost,
        $DBUser,
        $DBPassword,
        $DBName,
        $Connection,
        $DBStatus,
        $TablePrefix,
        $UseDB;

        function DatabaseInterface()
        {
            $this->DBHost='localhost';
            $this->DBUser='%DBUSER%';
            $this->DBPassword='%DBPASS%';
            $this->DBName='%DBNAME%';
            $this->DBStatus='Disconnected';
            $this->dbConnect();
        }

        function dbConnect()
        {
            if ($this->DBUser != '%UNKNOWUSERNAME%') {
            $this->Connection = mysql_connect($this->DBHost,$this->DBUser,$this->DBPassword);
            $this->UseDB = mysql_select_db($this->DBName);
            $this->DBStatus='Connected';
        }
            else
            {
                $this->DBStatus='Disconnected';
            }
        }

        function dbCloseConnection()
        {
            $this->DBStatus='Disconnected';
            $this->Connection = mysql_connect($this->DBHost,$this->DBUser,$this->DBPassword);
            mysql_close();
            //mysql_close(&$this->Connection);
        }

        function checkStatus()
        {
            return $this->DBStatus;
        }

        function selectQuery($tablename,$fields,$idfield=null,$identifier=null,$order=null,$limit=null,$groupby=null,$like=null,$wor=null,$extraf=null,$extraid=null)
        {
            if ($this->checkStatus() == 'Connected')
            {
            unset($fieldstr);
            //gets $fields from $tablename and returns $result
            foreach ($fields as $fieldname)
            {
                $fieldstr.=$fieldname.',';
            }

            $fieldstr=substr($fieldstr,0,-1);

            $sql='select '.$fieldstr.' from '.$tablename;

            if ((isset($idfield)) || (isset($extraf)))
            {
                $sql.=' where ';
            }

            if (isset($extraf))
            {
                $i=0;
                unset($extrafields);

                foreach ($extraf as $extra)
                {
                    $extrafields.= $extra.' = '.$extraid[$i].' and ';
                    $i++;
                }

                $sql.=$extrafields;
            }

            if (isset($idfield))
            {
                unset($where);

                if (isset($extraf))
                {
                    $where.='(';
                }

                $i=0;
                foreach($idfield as $idf)
                {
                    $where.=$idf;

                    if (isset($like))
                    {
                        $where .= ' LIKE \'';
                    }
                    else
                    {
                        $not = strpos($identifier[$i], '!');
                        $less = strpos($identifier[$i], '<');
                        $greater = strpos($identifier[$i], '>');

                        if ($not === false)
                        {
                            if ($less !== false)
                            {
                                $where .= ' < \'';
                                $identifier[$i] = trim($identifier[$i], '<');
                            }
                            elseif ($greater !== false)
                            {
                                $where .= ' > \'';
                                $identifier[$i] = trim($identifier[$i], '>');
                            }
                            else
                            {
                                $where .= ' = \'';
                            }
                        }
                        else
                        {
                            if ($not !== false)
                            {
                                $where .= ' != \'';
                                $identifier[$i] = trim($identifier[$i], '!');
                            }
                        }
                    }
                    $where .= $identifier[$i];
                    if (isset($wor))
                    {
                        $where .= '\' or ';
                    }
                    else
                    {
                        $where .= '\' and ';
                    }
                    $i++;
                }

                if (isset($wor))
                {
                    $where = substr($where,0,-3);
                }
                else
                {
                    $where=substr($where,0,-5);
                }

                if (isset($extraf))
                {
                    $where.=')';
                }

                $sql.=$where;
            }

            if (isset($groupby))
            {
                $sql.= " GROUP BY ".$groupby;
            }

            if (isset($order))
            {
                $sql.=" ORDER BY ".$order;
            }

            if (isset($limit))
            {
                $sql.=" LIMIT ".$limit;
            }
            $rs=mysql_query($sql);

			$result = array();

			if (!mysql_errno())
			{
				$result[0] = $rs;
	            $rnum = mysql_num_rows($rs);
				$result[1] = $rnum;
			}
			else
            {
				$result[1] = 0;
                $result[2] = 1;
				$result[3] = mysql_error();
				$result[4] = mysql_errno();
                $result[5] = $sql;
            }
            $result['query'] = $sql;

			$this->dbCloseConnection();
            }
            else
            {
                $result[1] = 0;
                $result[2] = 1;
                $resilt[3] = 'Error Database not connected';
                $result[4] = 'PHDNERR';
            }
            return $result;
        }

        function multipleSelectQuery($tablename1, $tablename2, $fields, $idfield = null, $identifier = null, $linkfields, $wor = null)
        {
            unset($fieldstr);

            foreach ($fields as $fieldname)
            {
                $fieldstr.=$fieldname.',';
            }

            $fieldstr=substr($fieldstr,0,-1);

            $sql = 'select ' . $fieldstr . ' from ' . $tablename1 . ' JOIN ' . $tablename2 . ' ON ';
            $sql .= $linkfields[0] . ' = ' . $linkfields[1];

            if (isset($idfield))
            {
                unset($where);
                $i=0;
                foreach($idfield as $idf)
                {
                    $where.=$idf;

                    $not = strpos($identifier[$i], '!');
                    $less = strpos($identifier[$i], '<');
                    $greater = strpos($identifier[$i], '>');

                    if ($not === false)
                    {
                        if ($less !== false)
                        {
                            $where .= ' < \'';
                            $identifier[$i] = trim($identifier[$i], '<');
                        }
                        elseif ($greater !== false)
                        {
                            $where .= ' > \'';
                            $identifier[$i] = trim($identifier[$i], '>');
                        }
                        else
                        {
                            $where .= ' = \'';
                        }
                    }
                    else
                    {
                        if ($not !== false)
                        {
                            $where .= ' != \'';
                            $identifier[$i] = trim($identifier[$i], '!');
                        }
                    }
                    $where .= $identifier[$i];
                    if (isset($wor))
                    {
                        $where .= '\' or ';
                    }
                    else
                    {
                        $where .= '\' and ';
                    }

                    $i++;
                }
                if (isset($wor))
                {
                    $where = substr($where,0,-3);
                }
                else
                {
                    $where=substr($where,0,-5);
                }

                $sql.= ' where ' . $where;
            }

            $rs=mysql_query($sql);

			$result = array();

			if (!mysql_errno())
			{
				$result[0] = $rs;
	            $rnum = mysql_num_rows($rs);
				$result[1] = $rnum;
			}
			else
            {
				$result[1] = 0;
                $result[2] = 1;
				$result[3] = mysql_error();
				$result[4] = mysql_errno();
                $result[5] = $sql;
            }
            $result['query'] = $sql;

			$this->dbCloseConnection();

            return $result;
        }

        function insertQuery($tablename,$fields,$fieldvalues)
        {
            unset($insstr);
            $i=0;
            foreach ($fields as $f)
            {
                $instr.=$f.',';
                $fldstr.='\''.$fieldvalues[$i].'\',';

                $i++;
            }

            $instr = substr($instr,0,-1);
            $fldstr = substr($fldstr,0,-1);

            $sql='insert into '.$tablename.'('.$instr.') values('.$fldstr.')';
            $rs=@mysql_query($sql,$this->Connection);
            
            if (!isset($rs))
            {
                //error in insert
                unset($result);
            }
            else
            {
                $result=1;
            }

            return $result;
        }

        function updateQuery($tablename,$fields,$fieldvalues,$idfield=null,$identifier=null)
        {
            //updates $tablename with $fields
            unset($updstr);
            $i=0;
            foreach ($fields as $f)
            {

                $updstr.=$f.'=\''.$fieldvalues[$i].'\',';
                $i++;
            }

            $updstr=substr($updstr,0,-1);

            $sql='update '.$tablename.' set '.$updstr;
			if (isset($idfield))
			{
			    if (!is_array($idfield))
                {
    				$sql.= ' where ' . $idfield . '=\'' . $identifier . '\'';
                }
                else
                {
                    unset($where);
                    $i=0;
                    foreach($idfield as $idf)
                    {
                        $where.=$idf;

                        $not = strpos($identifier[$i], '!');
                        $less = strpos($identifier[$i], '<');
                        $greater = strpos($identifier[$i], '>');

                        if ($not === false)
                        {
                            if ($less !== false)
                            {
                                $where .= ' < \'';
                                $identifier[$i] = trim($identifier[$i], '<');
                            }
                            elseif ($greater !== false)
                            {
                                $where .= ' > \'';
                                $identifier[$i] = trim($identifier[$i], '>');
                            }
                            else
                            {
                                $where .= ' = \'';
                            }
                        }
                        else
                        {
                            if ($not !== false)
                            {
                                $where .= ' != \'';
                                $identifier[$i] = trim($identifier[$i], '!');
                            }
                        }
                        $where .= $identifier[$i];
                        if (isset($wor))
                        {
                            $where .= '\' or ';
                        }
                        else
                        {
                            $where .= '\' and ';
                        }

                        $i++;
                    }
                    if (isset($wor))
                    {
                        $where = substr($where,0,-3);
                    }
                    else
                    {
                        $where=substr($where,0,-5);
                    }

                    $sql.= ' where ' . $where;
                }
			}
            
            $rs=mysql_query($sql,$this->Connection);
        
	        if (mysql_errno())
            {
                unset($result);
            }
            else
            {
                $result = 1;
            }

            return $result;
        }

        function selectRecordGroup($tablename,$fields,$idfield=null,$identifier=null)
        {
            //gets $fields as an array from $tablename
            unset($fieldstr);

            foreach ($fields as $fieldname)
            {
                $fieldstr.=$fieldname.',';
            }

            $fieldstr=substr($fieldstr,0,-1);

            $sql='select '.$fieldstr.' from '.$tablename;
            if (isset($idfield))
            {
                $sql.=' where '.$idfield.' = \''.$identifier.'\'';
            }

            $rs=@mysql_query($sql,$this->Connection);

            if (!$rs)
            {
                $error = '';//failed query
            }

            //read data from query and place in a readable returnable result.
            $result = $this->fetchRecordRows($rs);

            return $result;
        }

        /*function fetchRecordRows($inQuery, $arrtype=MYSQL_BOTH)
        {
            $result=array();
            mysql_data_seek($result, 0);
            while ($row = mysql_fetch_array($inQuery, $arrtype))
            {
                array_push($result, $row);
            }
            return $result;
        }*/

        function deleteRecord($tablename,$idfield,$identifier)
        {
            $sql = 'delete from '.$tablename;
            if (!is_array($idfield))
            {
                $sql .= ' where ' . $idfield.' = \''.$identifier.'\'';
            }
            else
            {
                unset($where);
                $i=0;
                foreach($idfield as $idf)
                {
                    $where.=$idf;

                    $not = strpos($identifier[$i], '!');
                    $less = strpos($identifier[$i], '<');
                    $greater = strpos($identifier[$i], '>');

                    if ($not === false)
                    {
                        if ($less !== false)
                        {
                            $where .= ' < \'';
                            $identifier[$i] = trim($identifier[$i], '<');
                        }
                        elseif ($greater !== false)
                        {
                            $where .= ' > \'';
                            $identifier[$i] = trim($identifier[$i], '>');
                        }
                        else
                        {
                            $where .= ' = \'';
                        }
                    }
                    else
                    {
                        if ($not !== false)
                        {
                            $where .= ' != \'';
                            $identifier[$i] = trim($identifier[$i], '!');
                        }
                    }
                    $where .= $identifier[$i];
                    if (isset($wor))
                    {
                        $where .= '\' or ';
                    }
                    else
                    {
                        $where .= '\' and ';
                    }

                    $i++;
                }
                if (isset($wor))
                {
                    $where = substr($where,0,-3);
                }
                else
                {
                    $where=substr($where,0,-5);
                }

                $sql.= ' where ' . $where;
            }
            $rs = mysql_query($sql);

            if (!mysql_errno())
            {
                $result[0] = 1;
            }
            else
            {
                $result[1] = 0;
                $result[2] = 1;
                $result[3] = mysql_error();
                $result[4] = mysql_errno();
            }

			return $result;
        }

        function runQuery($dbstr)
        {
            $rs = mysql_query($dbstr);

			if (!mysql_errno())
			{
				$result[0] = $rs;
			}
			else
            {
				$result[1] = 0;
                $result[2] = 1;
				$result[3] = mysql_error();
				$result[4] = mysql_errno();
            }

            //$this->dbCloseConnection();
            return $result;
        }

        function setup($statearray) {
            foreach ($statearray as $statement)
            {
                $res = $this->runQuery($statement);
            }
        }
    }
?>
