<?php

$tulind = new Tulind();

echo "version:" . $tulind->version();
echo "\r\n";

print_r($tulind->sma([1,2,3,4,5,6,7,8,9,10], [10]));
print_r($tulind->ema([25,24.875,24.781,24.594,24.5,24.625,25.219,27.25], [5]));

