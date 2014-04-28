use strict;
use Image::Magick;

my @images = ();
my @placedImages = ();
my $resultSize = 16;

sub findEmptySpace
{
	my ($width, $height) = @_;
	print "trying to place an $width x $height image...\n";
	while (1) {
		for my $x(0 .. $resultSize - $width) {
			for my $y(0 .. $resultSize - $height) {
				my $allowed = 1;
				for my $image(@placedImages) {
					if ($image->{x} + $image->{image}->Get("width") + 1  > $x && $image->{x} < $x + $width + 1 &&
						$image->{y} + $image->{image}->Get("height") + 1 > $y && $image->{y} < $y + $height + 1
					) {
						$allowed = 0;
					}
				}
				return ($x, $y) if $allowed;
			}
		}
		$resultSize *= 2;
		print "size increased to $resultSize\n";
	}
}

my ($path) = @ARGV;
opendir(my $dh, $path) || die;
while (my $file = readdir $dh) {
	if ($file =~ /\.png$/) {
		my $im = Image::Magick->new();
		$im->Read($path . "/" . $file);
		push @images, {name => $file, image => $im};
	}
}
closedir $dh;

@images = sort {(sort {$b <=> $a} ($b->{image}->Get("width"), $b->{image}->Get("height")))[0] <=> (sort {$b <=> $a} ($a->{image}->Get("width"), $a->{image}->Get("height")))[0]} @images;
print $_->{name}, "\n" for @images;

for my $image(@images) {
	my ($x, $y) = findEmptySpace($image->{image}->Get("width"), $image->{image}->Get("height"));
	$image->{x} = $x;
	$image->{y} = $y;
	print $image->{name}, " placed at $x $y\n";
	push @placedImages, $image;
}

my $resultImage = Image::Magick->new();
$resultImage->Set(size => $resultSize . 'x' . $resultSize);
$resultImage->ReadImage('canvas:transparent');
#my $index = $resultImage->colorAllocateAlpha(255, 255, 255, 127);
open (ANIMATIONS_FILE, '>' . $path . ".xml") || die;
print ANIMATIONS_FILE "<?xml version=\"1.0\" ?>\n";
for my $image(@placedImages) {
	$resultImage->Composite(
		image => $image->{image}, 
		x => $image->{x}, 
		y => $image->{y}, 
	);
	
	open (FILE, '>' . $path . '/' . $image->{name} . ".xml") || die;
	print FILE "<?xml version=\"1.0\" ?>\n" . 
		"<animation texture=\"" . $path . '/' . $path . ".png" . "\"" . 
		" x=\"" . $image->{x} . "\" y=\"" . $image->{y} . "\" w=\"" . $image->{image}->Get("width") . "\" h=\"" . $image->{image}->Get("height") . "\"" . 
		" nframes=\"1\" fps=\"0\" />";
	print ANIMATIONS_FILE "<animation file=\"" . $path . '/' . $image->{name} . ".xml" . "\"/>\n";
}
die $resultImage->Write(filename => $path . '/' . $path . ".png", compression => 'None');
# my $png_data = $resultImage->png(0);
# open (DISPLAY, '>' . $path . '/' . $path . ".png") || die;
# binmode DISPLAY;
# print DISPLAY $png_data;
# close DISPLAY;

#print @placedImages, "\n";
#print $resultSize;