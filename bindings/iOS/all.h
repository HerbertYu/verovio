#ifndef all_h
#define all_h
#import <VerovioFramework/beam.h>
#import <VerovioFramework/timeinterface.h>
#import <VerovioFramework/chord.h>
#import <VerovioFramework/pghead2.h>
#import <VerovioFramework/areaposinterface.h>
#import <VerovioFramework/harm.h>
#import <VerovioFramework/featureextractor.h>
#import <VerovioFramework/keyaccid.h>
#import <VerovioFramework/fing.h>
#import <VerovioFramework/functorparams.h>
#import <VerovioFramework/comparison.h>
#import <VerovioFramework/toolkit.h>
#import <VerovioFramework/rest.h>
#import <VerovioFramework/tuplet.h>
#import <VerovioFramework/plica.h>
#import <VerovioFramework/bboxdevicecontext.h>
#import <VerovioFramework/damage.h>
#import <VerovioFramework/beatrpt.h>
#import <VerovioFramework/expansionmap.h>
#import <VerovioFramework/page.h>
#import <VerovioFramework/bracketspan.h>
#import <VerovioFramework/reg.h>
#import <VerovioFramework/multirest.h>
#import <VerovioFramework/expansion.h>
#import <VerovioFramework/btrem.h>
#import <VerovioFramework/arpeg.h>
#import <VerovioFramework/zone.h>
#import <VerovioFramework/restore.h>
#import <VerovioFramework/linkinginterface.h>
#import <VerovioFramework/label.h>
#import <VerovioFramework/multirpt.h>
#import <VerovioFramework/mdiv.h>
#import <VerovioFramework/ftrem.h>
#import <VerovioFramework/labelabbr.h>
#import <VerovioFramework/pages.h>
#import <VerovioFramework/iodarms.h>
#import <VerovioFramework/rdg.h>
#import <VerovioFramework/breath.h>
#import <VerovioFramework/hairpin.h>
#import <VerovioFramework/staffgrp.h>
#import <VerovioFramework/pitchinflection.h>
#import <VerovioFramework/resources.h>
#import <VerovioFramework/text.h>
#import <VerovioFramework/space.h>
#import <VerovioFramework/vrv.h>
#import <VerovioFramework/sb.h>
#import <VerovioFramework/clef.h>
#import <VerovioFramework/layerdef.h>
#import <VerovioFramework/supplied.h>
#import <VerovioFramework/score.h>
#import <VerovioFramework/tie.h>
#import <VerovioFramework/gracegrp.h>
#import <VerovioFramework/ligature.h>
#import <VerovioFramework/symbol.h>
#import <VerovioFramework/metersig.h>
#import <VerovioFramework/surface.h>
#import <VerovioFramework/runningelement.h>
#import <VerovioFramework/expan.h>
#import <VerovioFramework/gliss.h>
#import <VerovioFramework/neume.h>
#import <VerovioFramework/boundingbox.h>
#import <VerovioFramework/abbr.h>
#import <VerovioFramework/floatingobject.h>
#import <VerovioFramework/devicecontextbase.h>
#import <VerovioFramework/pitchinterface.h>
#import <VerovioFramework/plistinterface.h>
#import <VerovioFramework/phrase.h>
#import <VerovioFramework/mrest.h>
#import <VerovioFramework/textelement.h>
#import <VerovioFramework/doc.h>
#import <VerovioFramework/layer.h>
#import <VerovioFramework/timemap.h>
#import <VerovioFramework/mordent.h>
#import <VerovioFramework/proport.h>
#import <VerovioFramework/choice.h>
#import <VerovioFramework/fermata.h>
#import <VerovioFramework/facsimile.h>
#import <VerovioFramework/tabdursym.h>
#import <VerovioFramework/pgfoot.h>
#import <VerovioFramework/f.h>
#import <VerovioFramework/editorial.h>
#import <VerovioFramework/measure.h>
#import <VerovioFramework/num.h>
#import <VerovioFramework/keysig.h>
#import <VerovioFramework/syllable.h>
#import <VerovioFramework/beamspan.h>
#import <VerovioFramework/editortoolkit_mensural.h>
#import <VerovioFramework/add.h>
#import <VerovioFramework/att.h>
#import <VerovioFramework/durationinterface.h>
#import <VerovioFramework/subst.h>
#import <VerovioFramework/staffdef.h>
#import <VerovioFramework/ending.h>
#import <VerovioFramework/devicecontext.h>
#import <VerovioFramework/lv.h>
#import <VerovioFramework/app.h>
#import <VerovioFramework/svg.h>
#import <VerovioFramework/section.h>
#import <VerovioFramework/trill.h>
#import <VerovioFramework/docselection.h>
#import <VerovioFramework/tabgrp.h>
#import <VerovioFramework/scoredef.h>
#import <VerovioFramework/view.h>
#import <VerovioFramework/options.h>
#import <VerovioFramework/facsimileinterface.h>
#import <VerovioFramework/layerelement.h>
#import <VerovioFramework/accid.h>
#import <VerovioFramework/controlelement.h>
#import <VerovioFramework/octave.h>
#import <VerovioFramework/iomusxml.h>
#import <VerovioFramework/git_commit.h>
#import <VerovioFramework/rend.h>
#import <VerovioFramework/pagemilestone.h>
#import <VerovioFramework/sic.h>
#import <VerovioFramework/grpsym.h>
#import <VerovioFramework/fig.h>
#import <VerovioFramework/runtimeclock.h>
#import <VerovioFramework/ioabc.h>
#import <VerovioFramework/fb.h>
#import <VerovioFramework/iomei.h>
#import <VerovioFramework/caesura.h>
#import <VerovioFramework/editortoolkit_cmn.h>
#import <VerovioFramework/mensur.h>
#import <VerovioFramework/stem.h>
#import <VerovioFramework/slur.h>
#import <VerovioFramework/mrpt.h>
#import <VerovioFramework/io.h>
#import <VerovioFramework/course.h>
#import <VerovioFramework/pghead.h>
#import <VerovioFramework/metersiggrp.h>
#import <VerovioFramework/custos.h>
#import <VerovioFramework/mrpt2.h>
#import <VerovioFramework/attalternates.h>
#import <VerovioFramework/attdef.h>
#import <VerovioFramework/unclear.h>
#import <VerovioFramework/mnum.h>
#import <VerovioFramework/dot.h>
#import <VerovioFramework/object.h>
#import <VerovioFramework/orig.h>
#import <VerovioFramework/artic.h>
#import <VerovioFramework/systemmilestone.h>
#import <VerovioFramework/pedal.h>
#import <VerovioFramework/iopae.h>
#import <VerovioFramework/instrdef.h>
#import <VerovioFramework/barline.h>
#import <VerovioFramework/transposition.h>
#import <VerovioFramework/nc.h>
#import <VerovioFramework/reh.h>
#import <VerovioFramework/system.h>
#import <VerovioFramework/elementpart.h>
#import <VerovioFramework/syl.h>
#import <VerovioFramework/annot.h>
#import <VerovioFramework/horizontalaligner.h>
#import <VerovioFramework/pb.h>
#import <VerovioFramework/pgfoot2.h>
#import <VerovioFramework/verticalaligner.h>
#import <VerovioFramework/lb.h>
#import <VerovioFramework/anchoredtext.h>
#import <VerovioFramework/editortoolkit_neume.h>
#import <VerovioFramework/del.h>
#import <VerovioFramework/tuning.h>
#import <VerovioFramework/smufl.h>
#import <VerovioFramework/pageelement.h>
#import <VerovioFramework/tempo.h>
#import <VerovioFramework/lem.h>
#import <VerovioFramework/staff.h>
#import <VerovioFramework/ref.h>
#import <VerovioFramework/editortoolkit.h>
#import <VerovioFramework/dynam.h>
#import <VerovioFramework/svgdevicecontext.h>
#import <VerovioFramework/mspace.h>
#import <VerovioFramework/turn.h>
#import <VerovioFramework/drawinginterface.h>
#import <VerovioFramework/systemelement.h>
#import <VerovioFramework/timestamp.h>
#import <VerovioFramework/glyph.h>
#import <VerovioFramework/dir.h>
#import <VerovioFramework/scoredefinterface.h>
#import <VerovioFramework/verse.h>
#import <VerovioFramework/iohumdrum.h>
#import <VerovioFramework/corr.h>
#import <VerovioFramework/note.h>
#import <VerovioFramework/positioninterface.h>
#import <VerovioFramework/halfmrpt.h>
#import <VerovioFramework/textdirinterface.h>
#import <VerovioFramework/vrvdef.h>
#import <VerovioFramework/MidiEvent.h>
#import <VerovioFramework/MidiFile.h>
#import <VerovioFramework/MidiEventList.h>
#import <VerovioFramework/Binasc.h>
#import <VerovioFramework/MidiMessage.h>
#import <VerovioFramework/crc.h>
#import <VerovioFramework/jsonxx.h>
#import <VerovioFramework/humlib.h>
#endif /* all_h */

