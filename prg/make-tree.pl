use Tree::Visualize;
use Tree::Binary;
use Tree::Parser;

# create a new parser object with some input
my $inputString = "";

# read tree string from STDIN 
while(<>){
   $inputString = $inputString.$_;
}

# init Tree::Parser object
my $input = Tree::Parser->prepareInput($inputString);
my $tp = Tree::Parser->new($input);

# use the built in space=1 indent filters, 
$tp->useSpaceIndentedFilters(1); 

# parse our input and get back a tree
my $tree = $tp->parse();

my $visualize = Tree::Visualize->new($tree, 'ASCII', 'TopDown');
print $visualize->draw();
