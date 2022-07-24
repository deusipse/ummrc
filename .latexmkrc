$latex = 'latex --shell-escape %O %S';
$pdflatex = 'pdflatex --shell-escape %O %S';

sub asy {return system("asy -o asy/ '$_[0]'");}
add_cus_dep("asy","eps",0,"asy");
add_cus_dep("asy","pdf",0,"asy");
add_cus_dep("asy","tex",0,"asy");
