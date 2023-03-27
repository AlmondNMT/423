# This tests a sample virtual pet type...as a module. Yeah, that's the ticket.

#   public static int hunger;
hunger : int
#   public static int happy;
happy : int

def play():
   print("What should we play?\n");
   print( "Fetch (1) \n Roll over (2)\n" );
   choice = 2
   if (choice == 1):
      print("Fetch, eh")
      happy = happy+10
   else:
      if (choice == 2):
         print("Roll over?  Really?")
         hunger=hunger+ 1

def main():
  play()
