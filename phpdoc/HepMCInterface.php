<html>
<head>
<title>HepMC Interface</title>
<link rel="stylesheet" type="text/css" href="pythia.css"/>
<link rel="shortcut icon" href="pythia32.gif"/>
</head>
<body>

<script language=javascript type=text/javascript>
function stopRKey(evt) {
var evt = (evt) ? evt : ((event) ? event : null);
var node = (evt.target) ? evt.target :((evt.srcElement) ? evt.srcElement : null);
if ((evt.keyCode == 13) && (node.type=="text"))
{return false;}
}

document.onkeypress = stopRKey;
</script>
<?php
if($_POST['saved'] == 1) {
if($_POST['filepath'] != "files/") {
echo "<font color='red'>SETTINGS SAVED TO FILE</font><br/><br/>"; }
else {
echo "<font color='red'>NO FILE SELECTED YET.. PLEASE DO SO </font><a href='SaveSettings.php'>HERE</a><br/><br/>"; }
}
?>

<form method='post' action='HepMCInterface.php'>

<h2>HepMC Interface</h2>

An interface to the HepMC [<a href="Bibliography.php" target="page">Dob01</a>] standard event record 
format has been provided by M. Kirsanov. To use it, the relevant 
libraires need to be linked, as explained in the <code>README</code> 
file. Only version 2 of HepMC is supported. (Version 1 requires 
a different interface structure, which was only supported up until 
Pythia 8.107.) 

<p/>
The (simple) procedure to translate PYTHIA 8 events into HepMC ones 
is illustrated in the <code>main31.cc</code>, <code>main32.cc</code>   
and <code>main54.cc</code> main programs. At the core is a call to
the
<pre>
HepMC::I_Pythia8::fill_next_event( pythia, hepmcevt, ievnum = -1, convertGluonTo0 = false ) 
</pre>
which takes a reference of the generator object and uses it, on the one
hand, to read out and covert the event record in <code>pythia.event</code> 
and, on the other hand, to extract and store parton-density (PDF) 
information for the hard subprocess from <code>pythia.info</code>. 
The optional last argument, if <code>true</code>, allows you to store 
gluons as "PDG" code 0 rather than the normal 21; this only applies to 
the PDF information, not the event record. 

<p/>
The earlier version of this routine,
<pre>
HepMC::I_Pythia8::fill_next_event( pythia.event, hepmcevt, ievnum = -1 ) 
</pre>
is retained (for now) for backwards compatibility. It takes a PYTHIA event 
as input and returns a HepMC one, but without storing the PDF information. 
The latter could then instead be stored by a separate call
<pre>
HepMC::I_Pythia8::pdf_put_info( hepmcevt, pythia, convertGluonTo0 = false ) 
</pre>
or not, as wished.

<p/>
The translation routine stores momenta, energies and masses in units 
of GeV, and distances and times in units of mm, with <i>c = 1</i>. 
This is exactly the units used in PYTHIA. Starting with HepMC 2.04
the units can be specified; this will be implemented once this or
a later version becomes used by the experimental community.

<p/>
The status code is now based on the proposed standard for HepMC 2.05,
see the <?php $filepath = $_GET["filepath"];
echo "<a href='EventRecord.php?filepath=".$filepath."' target='page'>";?>Event::statusHepMC(...)</a> 
conversion routine for details. The earlier behaviour, where all
final particles had status 1 and all initial or intermediate ones 
status 2, is available as a commented-out line in the   
<code>I_Pythia8::fill_next_event(...)</code> method, and so is simple
to recover.

<p/>
In HepMC 2.05 it also becomes possible to store the generated cross 
section and its error. The environment variable
<code>HEPMC_HAS_CROSS_SECTION</code> is used to check whether this
possibility exists and, if it does the current values from 
<code>pythia.info.sigmaGen()</code> and 
<code>pythia.info.sigmaErr()</code> are stored for each event,
multiplied by <i>10^9</i> to convert from mb to pb. Note that
PYTHIA improves it accuracy by Monte Carlo integration in the course
of the run, so the values associated with the last generated event
should be the most accurate ones. 

<h2>The public methods</h2>

Here comes a complete list of all public methods of the 
<code>I_Pythia8</code> class in the <code>HepMC</code> 
(<i>not</i> <code>Pythia8</code>!) namespace.

<a name="method1"></a>
<p/><strong>I_Pythia8::I_Pythia8() &nbsp;</strong> <br/>
  
<strong>virtual I_Pythia8::~I_Pythia8() &nbsp;</strong> <br/>
the constructor and destructor take no arguments.
  

<a name="method2"></a>
<p/><strong>bool I_Pythia8::fill_next_event( Pythia8::Pythia& pythia, GenEvent* evt, int ievnum = -1, bool convertGluonTo0 = false) &nbsp;</strong> <br/>
convert a <code>Pythia</code> event to a <code>HepMC</code> one.
Will return true if succeeded.
<br/><code>argument</code><strong> pythia </strong>  : 
the input <code>Pythia</code> generator object, from which both the 
event and the parton density information can be obtained.
   
<br/><code>argument</code><strong> evt </strong>  : 
the output <code>GenEvt</code> event, in its standard form.
   
<br/><code>argument</code><strong> iev </strong>  : 
set the event number of the current event. If negative then the 
internal event number is used, which is incremented by one for
each new event.
  
<br/><code>argument</code><strong> convertGluonTo0 </strong>  : 
the normal gluon identity code 21 is used also when parton density
information is stored, unless this optional argument is set true to
have gluons represented by a 0. This choice does not affect the 
normal event record, where a gluon is always 21. 
  
  

<a name="method3"></a>
<p/><strong>bool I_Pythia8::fill_next_event( Pythia8::Event& pyev, GenEvent* evt, int ievnum = -1 ) &nbsp;</strong> <br/>
convert a <code>Pythia</code> event to a <code>HepMC</code> one.
Will return true if succeeded. Do not store parton-density information.
<br/><code>argument</code><strong> pyev </strong>  : 
the input <code>Pythia</code> event, in its standard form.
   
<br/><code>argument</code><strong> evt </strong>  : 
the output <code>GenEvt</code> event, in its standard form.
   
<br/><code>argument</code><strong> iev </strong>  : 
set the event number of the current event. If negative then the 
internal event number is used, which is incremented by one for
each new event.
  
  

<a name="method4"></a>
<p/><strong>bool I_Pythia8::put_pdf_info( GenEvent* evt, Pythia8::Pythia& pythia, bool convertGluonTo0 = false ) &nbsp;</strong> <br/>
append parton-density information to an event already stored
by the previous method.
<br/><code>argument</code><strong> evt </strong>  : 
the output <code>GenEvt</code> event record, in its standard form.
   
<br/><code>argument</code><strong> pythia </strong>  : 
the input <code>Pythia</code> generator object, from which both the 
event and the parton density information can be obtained.
   
<br/><code>argument</code><strong> convertGluonTo0 </strong>  : 
the normal gluon identity code 21 is used also when parton density
information is stored, unless this optional argument is set true to
have gluons represented by a 0. 
  
  

<a name="method5"></a>
<p/><strong>void I_Pythia8::set_trust_mothers_before_daughters( bool b = 1 ) &nbsp;</strong> <br/>
  
<strong>bool I_Pythia8::trust_mothers_before_daughters() &nbsp;</strong> <br/>
if there is a conflict in the history information, then trust the 
information on mothers above that on daughters. Currently this is
the only option implemented.
  

<a name="method6"></a>
<p/><strong>void I_Pythia8::set_trust_both_mothers_and_daughters( bool b = 0 ) &nbsp;</strong> <br/>
  
<strong>bool I_Pythia8::trust_both_mothers_and_daughters() &nbsp;</strong> <br/>
currently dummy methods intended to resolve conflicts in the event
history.
  

<a name="method7"></a>
<p/><strong>void I_Pythia8::set_print_inconsistency_errors( bool b = 1 ) &nbsp;</strong> <br/>
  
<strong>bool I_Pythia8::print_inconsistency_errors() &nbsp;</strong> <br/>
print a warning line, on <code>cerr</code>, when inconsistent mother 
and daughter information is encountered.
  

<a name="method8"></a>
<p/><strong>void I_Pythia8::set_crash_on_problem( bool b = 1 ) &nbsp;</strong> <br/>
if problems are encountered then the run is interrupted by an
<code>exit(1)</code> command. Default is not to crash.
  

<a name="method9"></a>
<p/><strong>void I_Pythia8::set_convert_to_mev( bool b = 1 ) &nbsp;</strong> <br/>
connvert the normal GeV energies, momenta and masses to MeV.
Currently not implemented, so everything always is stored as GeV.
  

</body>
</html>

<!-- Copyright (C) 2010 Torbjorn Sjostrand -->
