 ===============================================================================
 =                                                                             =
 =  QWait 0.6 beta                                                             =
 =                                                                             =
 =  July 9th 2022                                                              =
 =                                                                             =
 =  www.freshney.org // paul@freshney.org                                      =
 =                                                                             =
 =  Application and Source Code:                                               =
 =  https://github.com/MaximumOctopus/QWait                                    =
 =                                                                             =
 ===============================================================================

 Hello!
 
 This was developed after watching a youtube video which discussed the differences
 in queue time between FastPass+ and non-FastPass enabled theme parks, when modelled.
 
 I thought it would be an interesting project to build my own simulation and see
 what came out.
 
 I've relied heavily on internet sources for much of the data regarding theme park
 operation (such as ride length). Without this information a simulation like this
 would be much harder to build. Where I couldn't find information I made an educated
 guess. I've listed the sources at the bottom of this document and in the source
 code where they are used.
 
 The information for the six parks supplied should be accurate enough.
 
 This isn't a perfect simulation, but I think for a first release it's "close enough".
 
 Unlike the simulation in the video mentioned above, my version does have visitors
 which travel from ride to ride. It's a small addition but I think it's important.
 
 Visitors won't get hungry, thirsty, or tired.
 
 It's written in C++ with Visual Studio 2022, but should compile with any
 C++ compiler on (I think) any platform. If you notice something platform-dependent
 then let me know.
 
   Example usage:
 
     qwait /visitors;30000 /template;1
 
 Many thanks,

 Paul A Freshney (paul@freshney.org)

========================================================================
========================================================================

 Credits:

   All coding       : Paul A Freshney
   Development Cats : Rutherford, Freeman, and Maxwell

   Dedicated to Julie, Adam, and Dyanne.
   
========================================================================
========================================================================

 Sources   

  https://wdwthemeparks.com/hourly-capacity-numbers/
  https://www.wdwinfo.com/wdwinfo/ridelength.htm
  https://crooksinwdw.wordpress.com/2013/12/14/theoreticaloperational-hourly-ride-capacity-at-wdw/
  https://eu.azcentral.com/story/travel/destinations/california/2019/08/16/disneyland-ride-capacity-how-long-will-you-have-wait-line/1927184001/

  https://www.mousehacking.com/blog/best-animal-kingdom-rides
  https://www.mousehacking.com/blog/every-walt-disney-world-ride-ranked
  https://magicguides.com/best-epcot-attractions/
  https://www.mousehacking.com/blog/disney-world-height-requirements
 
  https://www.disneytouristblog.com/paid-lightning-lanes-genie-replace-free-fastpass-disney-world/

  https://www.wdwinfo.com/wdwinfo/fastpass.htm
  https://forums.wdwmagic.com/threads/percentage-of-fps-allocated-for-rides-wdw.955357/
  https://www.ocregister.com/2020/05/22/what-an-attendance-cap-could-mean-for-crowded-disney-parks-in-the-covid-19-era/
  
  https://towersstreet.com/theme-park/ride/
  https://www.themeparkjames.co.uk/theme-parks/europe/uk/alton-towers/
  https://www.youtube.com/watch?v=cVUlHjQximM
  
========================================================================
== Updates for 0.6 beta ================================================
========================================================================

This is a major update, which after three weeks of work, is ready to release.

In order to better reflect average theme park visitors, I've altered the
engine to use groups of individuals roaming together (family, couple,
adult group, single), instead of thousands of independent agents (which
isn't releastic at all!).

There is some tweaking required, and likely a bug here or there, but
it should offer better results than the previous version.

This has meant rewriting quite a lot of code, and most output files
created before this (templates, visitor demographic file, etc.) are no
longer compatible. I've updated all included template files.

Rides now have the option of accepting any of: babies, children, and adults.

I've added a few new reports, improved existing reports, and added more
debug output options.

========================================================================
== Updates for 0.5 beta ================================================
========================================================================

Added data for the UK's most popular park, Alton Towers.

========================================================================
== Updates for 0.4 beta ================================================
========================================================================

Added: HTML report output

========================================================================
== Updates for 0.3 beta ================================================
========================================================================

Added: Ability to load park templates from a file.
Added: Template files for all parks based on real-life data.
Added: Ability to save randomly generated visitor data
Added: Ability load previously saved visitor data 
       (this is useful if you want to maintain a known set of data across
	   multiple simulations)
	   
Quite a few tidies and tweaks.	   

========================================================================
== Updates for 0.2 beta ================================================
========================================================================

First release ;)
