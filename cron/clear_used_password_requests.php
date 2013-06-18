<?php
    include('/home/eatinoro/public_html/devbeta/App.kfc');
    include('/home/eatinoro/public_html/devbeta/includes/class.phpmailer.php');
    include('/home/eatinoro/public_html/devbeta/includes/functions.inc');
    include('/home/eatinoro/public_html/devbeta/includes/Class_DatabaseInterface2.inc');

    $idfield = 'used';
    $idval = '1';

    $DBA = new DatabaseInterface();
    $rs = $DBA->deleteRecord(DBPWLINKTABLE, $idfield, $idval);

    unset($idfield, $idval, $rs, $DBA);
?>