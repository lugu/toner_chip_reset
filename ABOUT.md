
Background
==========

Few months ago i found this super promotion : a Ricoh SP112 laser
printer for just 19€. So I told myself:

> "a *Laser* printer that cheap, what an opportunity!"

But when i received it i quickly realize it is a
[GDI printer](http://www.openprinting.org/printer/Generic/Generic-GDI_Printer)
and there is not driver for Linux :(

To quote Wikipedia:

> A GDI printer or Winprinter (analogous to a Winmodem) is a printer
> designed to accept output from a host computer running the GDI under
> Windows.

So i started an adventure with qemu, tcpdump, cups filters and image
magick and after a few nights on it, i could proudly print on Linux.

One thing the Linux driver was missing was the "dot count". This is a
value sent by the PC host to the number to indicate the number of
black dots in the images. I wasn't sure what this is for but for the
sake of completeness, i added it to the driver.

Unfortunatly, after a few more prints, the printer failed indicating
that I must change the toner. On this printer, like on many other,
the toner is not just a bag of ink, it includes mechanical part and
even a small electronic circuit.

The best price online for an SP112 toner was 35 euros! Well, this is
184% of the price of the printer. No way i am going to spend that much
for a 20 bucks printer. And no way i am going to buy a new printer
each time the toner get empty.

When i opened to toner, i realized it is full of pouder ink. It was
not empty as the printer says. And the toner by itself had no sensor
to detect the ink level. At this stage i realized why this "dot count"
was sent : it is used to tell this dummy printer how much ink would be
consumed.
