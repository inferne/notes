<?php 

echo ("aaa"+0);

$mysqli = mysqli_connect('localhost', 'root', '', 'yc_crm_common', 3306);

if(mysqli_connect_errno()){
    printf("Connect failed:%s\n");
    exit();
}

$sql = 'select * from driver where driver_id = ?';
$stmt = $mysqli->prepare($sql);
$stmt->bind_param('d', $id);
$id = 62;
$stmt->execute();

// ok
// $stmt->bind_result($driver_id, $company_id);
// $stmt->fetch();
// echo $driver_id,$company_id;

// failed
// $result = $stmt->result_metadata();
// $field = $result->fetch_field();
// var_dump($field);//object
// echo $field->driver_id,$field->company_id;
// $result->close();

// $result = $stmt->store_result();
// var_dump($result);//true
// echo $stmt->num_rows;//1

$result = $stmt->get_result();
var_dump($result);//object(mysqli_result)
$row = $result->fetch_array(MYSQLI_NUM);
print_r($row);//Array ( [0] => 62 [1] => 11 )
var_dump($row);
$stmt->close();
mysqli_close($mysqli);