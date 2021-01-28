########################################################################
# 2. DEFINE YOUR CONVOLUTIONAL NEURAL NETWORK
########################################################################

import torch.nn as nn
import torch.nn.functional as F


class ConvNet(nn.Module):
    def __init__(self, init_weights=False):
        super(ConvNet, self).__init__()
        #INITIALIZE LAYERS HERE
        self.conv1 = nn.Conv2d(3, 64, 5)
        self.dropout = nn.Dropout(p=0.2)
        self.bn2d1 = nn.BatchNorm2d(64)
        self.pool = nn.MaxPool2d(2, 2)
        self.conv2 = nn.Conv2d(64, 64, 5)
        self.bn2d2 = nn.BatchNorm2d(64)
        
        self.dense1 = nn.Linear(64*5*5, 120)
        self.dense2 = nn.Linear(120, 84)
        self.dense3 = nn.Linear(84, 10)
        
        
        if init_weights:
            self._initialize_weights()

    def forward(self, x):
        #PASS IMAGE X THORUGH EACH LAYER DEFINED ABOVE
        x = self.pool(F.relu(self.bn2d1(self.conv1(x))))
        x = self.pool(F.relu(self.bn2d2(self.dropout(self.conv2(x)))))
        x = x.view(-1, 64*5*5)
        x = (self.dense1(x))
        x = (self.dense2(x))
        out = (self.dense3(x))
        return out

    def _initialize_weights(self):
        for m in self.modules():
            if isinstance(m, nn.Conv2d):
                nn.init.kaiming_normal_(m.weight, mode='fan_out', nonlinearity='relu')
                if m.bias is not None:
                    nn.init.constant_(m.bias, 0)
            elif isinstance(m, nn.BatchNorm2d):
                nn.init.constant_(m.weight, 1)
                nn.init.constant_(m.bias, 0)
            elif isinstance(m, nn.Linear):
                nn.init.normal_(m.weight, 0, 0.01)
                nn.init.constant_(m.bias, 0)

