<?php
$network_prefix="131.174";

# bash shell helpers 
#
#    getipfrommac() { curl "http://www.cs.ru.nl/lab/dyndns/?mac=$1&cmdline=1" 2>/dev/null }
#    setmac2ip() { curl "http://www.cs.ru.nl/lab/dyndns/?mac=$1&ip=$2&cmdline=1" 2>/dev/null;  }
#
# usage:
#   $ setmac2ip c8:1f:66:bb:5f:2c 131.174.30.57
#   $ getipfrommac c8:1f:66:bb:5f:2c
#   131.174.30.57
#
# thus:
#    run setmac2ip when booting machine after getting ip
#    then othermachines can use the mac address to get ip of that machine  

$via_form = true;
$verbose = false;
$print_all_data = false; 


$formdata = <<<HERE
<h1> Lookup ip using mac address </h1>
<form action="index.php" method="get">
  Mac address:  <input type="text" name="mac"><br>
  <br/>
  <input type="submit" value="Lookup IP">
</form> 
HERE;



// src: http://technologyordie.com/php-mac-address-validation
function format_mac($mac, $format='linux'){
 
    $mac = trim($mac);
    $len = strlen($mac);
    $mac = preg_replace("/[^a-fA-F0-9:]/",'',$mac);
    if ( $len != strlen($mac) ) 
        return false;  // removed illegal character
 
    $mac = preg_replace("/[^a-fA-F0-9]/",'',$mac);

    $mac = (str_split($mac,2));
    if(!(count($mac) == 6))
        return false;
 
    if($format == 'linux' || $format == ':'){
        return $mac[0]. ":" . $mac[1] . ":" . $mac[2]. ":" . $mac[3] . ":" . $mac[4]. ":" . $mac[5];
    }elseif($format == 'windows' || $format == '-'){
        return $mac[0]. "-" . $mac[1] . "-" . $mac[2]. "-" . $mac[3] . "-" . $mac[4]. "-" . $mac[5];
    }elseif($format == 'cisco'){
        return $mac[0] . $mac[1] . ":" . $mac[2] . $mac[3] . ":" . $mac[4] . $mac[5];
    }else{
        return $mac[0]. "$format" . $mac[1] . "$format" . $mac[2]. "$format" . $mac[3] . "$format" . $mac[4]. "$format" . $mac[5];
    }
}
function errorexit($msg) {
    global $formdata,$via_form;
    if ( $via_form ) {
       echo "<p style='color:#FF0000'>$msg</p>";
       echo "$formdata</center></html>"; 
    } else{
       echo $msg;
    }
    exit(0);
}


if ( $_GET["cmdline"] ) $via_form =false;

$mac= $_GET["mac"];
$ip= $_GET["ip"];

if ( $via_form )  echo "<html><br/><br/><br/><center>"; 



if ( ! $mac   ) {
    errorexit("missing mac");
}    



$mac=format_mac($mac,'linux'); // returns false if not valid mac
if (  ! $mac   )  {
    errorexit("invalid mac");
} 
if (  $ip != "" ) {
    $ip = trim($ip);
    $ip=filter_var($ip, FILTER_VALIDATE_IP); //return false if not valid ip
    if ( !  $ip  )  {
        errorexit("invalid ip");
    } 
    if (  substr( $ip, 0, strlen($network_prefix)  ) != "$network_prefix"  ) {
        errorexit("invalid ip, must be within network $network_prefix.*.*");
    } 
}  


$datafile='/tmp/labdata.txt';
$oldstring = file_get_contents($datafile);
$data= unserialize( $oldstring );




if (  ! $ip  ) {
  #get
  $ip=$data["$mac"];
  if  ( ! $ip ) $ip="nothing found";
  if ( $via_form ) {
      echo "$formdata"; 
      echo "<br><br><h2> Result:   <br><br>  $ip</h2>"; 
  } else {
      echo $ip;
  }
} else {
  #set
  $data["$mac"]=$ip;


  $newstring = serialize( $data ); 
  file_put_contents($datafile, $newstring, LOCK_EX);
  
  if ( $via_form ) {
      echo "$formdata"; 
      echo "<br><br><h2> Update:   <br><br> mac=$mac to ip=$ip</h2>"; 
  } else {
      echo "update mac=$mac to ip=$ip";
  }
  if ( $verbose ) { 
      echo "<br>";
      echo "mac:" . $mac;
      echo "<br>";
      echo "ip:" . $ip;
  }
}

# verbose
if ( $print_all_data ) { 
  if ( $via_form ) {
       echo "<br>"; 
       echo "<br>"; 
       var_dump($data);
  } else {     
       echo "\n";  
       echo "\n";  
       print_r($data);
  }
}

if ( $via_form ) echo "</center></h1></html>"; 


