<?php
  include 'database.php';
  if (!empty($_POST)) {
    $sensorName = $_POST['sensorName'];
    $myObj = (object)array();
    $pdo = Database::connect();
    $sql = 'SELECT * FROM esp32_record where sensorName = "'. $sensorName . '" order by date desc, time desc limit 1';
    $q = $pdo->prepare($sql);
    $q->execute();
    $row = $q->fetch();
    if ($row==null) 
      {      $myJSON = json_encode($myObj);
      echo $myJSON;
}
    else{
      $date = date_create($row['date']);
      $dateFormat = date_format($date,"d-m-Y");
      $time = date_create($row['time']);
      $timeFormat = date_format($time,"H:i");      
      $myObj->id = $row['id'];
      $myObj->sensorName = $row['sensorName'];
      $myObj->temperature = $row['temperature'];
      $myObj->humidity = $row['humidity'];
      $myObj->battery = $row['battery'];
      $myObj->RSSI = $row['RSSI'];
      $myObj->ls_time = $timeFormat;
      $myObj->ls_date = $dateFormat;
      $myJSON = json_encode($myObj);
      echo $myJSON;
      // echo $sql;
    }
    Database::disconnect();
  }
?>