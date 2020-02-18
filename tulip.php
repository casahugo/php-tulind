<?php

$open  = [4,5,5,5,4,4,4,6,6,6];
$high  = [9,7,8,7,8,8,7,7,8,7];
$low   = [1,2,3,3,2,1,2,2,2,3];
$close = [4,5,6,6,6,5,5,5,6,4];
$volume = [123,232,212,232,111,232,212,321,232,321];

$real = [1,2,3,4,5,6,7,8,9,10];

$tulind = new Tulind();

echo "version:" . $tulind->version();
echo "\r\n";

print_r($tulind->sma([$real], [2]));
print_r($tulind->stoch([$high, $low, $close], [5,3,3]));

