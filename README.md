#Weight File Path

Path : Caffe\windows\caffe\\.

##Weight file
* resnet-18 : standard residual network layer 18<br>
* resnet_pretrain : pretraining result using leap motion sdk<br>
(single - using left side cam, else - using both)<br>
* resnet_classification : training after pretrain. <br>

##prototxt
* solver : training parameter file ( ex. learinin rate, weight decay, test phase interval, path to training result..)<br>
* deploy : using demonstration.<br>
* other : training network structure. ( ex.resnet_18_solver_classification_single.prototxt)<br>

##snapshot
* *.caffemodel : weight file. if u want using some another project or finetuning, using this file.<br>
* *.solverstate : weight file + training state. if u want training more iteration, using this file.<br>
