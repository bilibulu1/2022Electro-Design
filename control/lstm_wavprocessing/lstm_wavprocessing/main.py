
import tensorflow as tf
from pylab import*
import os
from sklearn.preprocessing import MinMaxScaler
import scipy.io.wavfile as wav
import librosa
import numpy
from python_speech_features import mfcc
tf.compat.v1.disable_eager_execution()
config = tf.compat.v1.ConfigProto()
config.gpu_options.allow_growth = True
sess = tf.compat.v1.Session(config=config)
def def_wav_read_mfcc(file_name):
    try:
        fs, audio = wav.read(file_name)
        processed_audio = mfcc(audio, samplerate=fs, nfft = 2048)
    except ValueError:
        audio, fs = librosa.load(file_name)
        processed_audio = mfcc(audio, samplerate=fs, nfft = 2048)
    return processed_audio
# 训练数据的文件夹
file_path = r'C:/Users/Lenovo/Desktop/data/301 - Crying baby'
# 路径拼接
data = [os.path.join(file_path,i) for i in os.listdir(file_path)]
# 定义每个文件的标签
label = [[0], [0], [0], [0], [0], [0], [0], [0], [0], [0],
         [0], [0], [0], [0], [0], [0], [0], [0], [0], [0],
         [0], [0], [0], [0], [0], [0], [0], [0], [0], [0],
         [0], [0], [0], [0], [0], [0], [0], [0], [0], [0],
         [0], [0], [0], [0], [0], [0], [0], [0], [0], [0],
         [0], [0], [0], [0], [0], [0], [0], [0], [0], [0],
         [0], [0], [0], [0], [0], [0], [0], [0], [0], [0],
         [0], [0], [0], [0], [0], [0], [0], [0], [0], [0],
         [0], [0], [0], [0], [0], [0], [0], [0], [0], [0],
         [0], [0], [0], [0], [0], [0], [0], [0], [0], [0],
         [0], [0], [0], [0], [0], [0], [0], [0], [1], [1],
         [1], [1], [1], [1], [1], [1], [1], [1], [1], [1],
         [1], [1], [1], [1], [1], [1], [1], [1], [1], [1],
         [1], [1], [1], [1], [1], [1], [1], [1], [1], [1],
         [1], [1], [1], [1], [1], [1], [1], [1], [1], [1],
         [1], [1], [1], [1], [1], [1], [1], [1], [1], [1],
         [1], [1], [1], [1], [1], [1], [1], [1], [1], [1],
         [1], [1], [1], [1], [1], [1], [1], [1], [1], [1],
         [1], [1], [1], [1], [1], [1], [1], [1], [1], [1],
         [1], [1], [1], [1], [1], [1], [1], [1], [1], [1],
         ]

def get_batch_for_train():
    Wav = []
    for j in range(0,200):
        wav = def_wav_read_mfcc(data[j])
        wav1 = wav[:499]
        Wav.append(wav1)
        print("Conversion to MFCC：",j+1)
    label_for_train = label[0:200]
    return Wav, label_for_train
Wav,label_for_train = get_batch_for_train()
n_inputs = 13
max_time = 499
lstm_size = 78
n_classes = 1
batch_size = 20
n_batch = len(Wav) // batch_size
nums_samples = len(Wav)
n = 499
print(nums_samples)
# 对原始的mfcc数据进行归一化
for i in range(len(Wav)):
    scaler = MinMaxScaler(feature_range=(0, 1))
    Wav[i] = scaler.fit_transform(Wav[i])
    print("scaler:",i)
for i in range(len(Wav)):
    Wav[i] = Wav[i].tolist()
print(Wav)
print(label_for_train)
Wav_tensor = tf.compat.v1.convert_to_tensor(Wav)
label_tensor = tf.compat.v1.convert_to_tensor(label_for_train)
print("Success construct Wav_tensor")
# 生成的 Wav[] 里面放的是 array(15000×13)因此用循环将向量转换成列表

def get_batch(data, label, batch_size):
    input_queue =  tf.compat.v1.data.Dataset.from_tensor_slices(data).repeat()
    input_queue_y =  tf.compat.v1.data.Dataset.from_tensor_slices(label).repeat()
    x_batch = input_queue.batch(batch_size)
    y_batch = input_queue_y.batch(batch_size)
    batch_x = tf.compat.v1.data.make_one_shot_iterator(x_batch)
    x_batch = batch_x.get_next()
    batch_y = tf.compat.v1.data.make_one_shot_iterator(y_batch)
    y_batch = batch_y.get_next()
    return x_batch, y_batch

x_batch, y_batch = get_batch(Wav_tensor, label_tensor, batch_size)
x_batch_test, y_batch_test = get_batch(Wav_tensor, label_tensor, 100)
x_batch =  tf.compat.v1.convert_to_tensor(x_batch)
y_batch =  tf.compat.v1.convert_to_tensor(y_batch)

x =  tf.compat.v1.placeholder( tf.compat.v1.float32, [None, 499, 13])
y =  tf.compat.v1.placeholder( tf.compat.v1.float32, [None, 1])

def lstm_model(X, weights, biases):
    inputs =  tf.compat.v1.reshape(X, [-1, max_time, n_inputs])
    lstm_cell_1 = tf.keras.layers.LSTMCell(lstm_size)
    outputs_1, final_state_1=  tf.compat.v1.nn.dynamic_rnn(lstm_cell_1, inputs, dtype= tf.compat.v1.float32)
    lstm_cell_2 = tf.keras.layers.LSTMCell(lstm_size)
    outputs_2, final_state_2=  tf.compat.v1.nn.dynamic_rnn(lstm_cell_2, outputs_1, dtype= tf.compat.v1.float32)
    lstm_cell_3 = tf.keras.layers.LSTMCell(lstm_size)
    outputs_3, final_state_3=  tf.compat.v1.nn.dynamic_rnn(lstm_cell_3, outputs_2, dtype= tf.compat.v1.float32)
    lstm_cell_4 = tf.keras.layers.LSTMCell(13)
    outputs, final_state=  tf.compat.v1.nn.dynamic_rnn(lstm_cell_4, outputs_3, dtype= tf.compat.v1.float32)
    result = tf.nn.sigmoid( tf.compat.v1.matmul(final_state[0], weights) + biases)
    return result
model = tf.keras.models.load_model('./save1/model.ckpt-300.meta')
weights =  tf.compat.v1.Variable( tf.compat.v1.truncated_normal([13, n_classes], stddev=0.1))
biases =  tf.compat.v1.Variable( tf.compat.v1.constant(0.1, shape=[n_classes]))
prediction = lstm_model(x,weights,biases)
cross_entropy =  tf.compat.v1.reduce_mean( tf.compat.v1.square(y - prediction),name ="cross_entropy" )
train_step =  tf.compat.v1.train.AdamOptimizer(1e-3).minimize(cross_entropy)
correct_prediction =  tf.compat.v1.equal(y,tf.round(prediction))
accuracy =  tf.compat.v1.reduce_mean( tf.compat.v1.cast(correct_prediction, tf.compat.v1.float32),name = "accuracy")
init =  tf.compat.v1.global_variables_initializer()
config =  tf.compat.v1.ConfigProto()
config.gpu_options.allocator_type = "BFC"


saver =  tf.compat.v1.train.Saver()
with  tf.compat.v1.Session() as sess:
    with  tf.compat.v1.device('/gpu:0'):
        sess.run(init)
        loss = []
        checkpoint_steps = 100
        for i in range(600):
            x_batch_data = sess.run(x_batch)
            y_batch_data = sess.run(y_batch)
            x_batch_test_data = sess.run(x_batch_test)
            y_batch_test_data = sess.run(y_batch_test)
            sess.run(train_step, feed_dict={x: x_batch_data, y: y_batch_data})
            pred_X1 = sess.run(prediction, feed_dict={x: x_batch_data})
            pred_X1 = pred_X1[0]
            if pred_X1 >= 0.5:
                print("This sound is True.")
            else:
                print("This sound is False.")
            y_batch_data = y_batch_data[0]
            if y_batch_data[0] >= 0.5:
                y_r = True
            else:
                y_r = False
            print("prediction",i,":",prediction,";  ","real value：",y_batch_data,";  ","test result:",pred_X1)
            print("test results:",pred_X1,";  ","real value:",y_batch_data)
            if (i + 1) % 10 == 0:
                cross_entropy_new = sess.run(cross_entropy,feed_dict={x: x_batch_test_data, y: y_batch_test_data})
                accurace = sess.run(accuracy,feed_dict={x: x_batch_test_data, y: y_batch_test_data})
                loss.append(cross_entropy_new)
                print("accurace:",cross_entropy_new,accurace,"Iteration",i+1)

            if (i + 1) % checkpoint_steps == 0:
                saver.save(sess, "save1/model.ckpt", global_step=i + 1)
                print("Success save.")
    k = len(loss)
    t = []
    for i in range(k):
        t.append(i)
    print(t)
    plt.plot(t, loss, 'k-', label='Data', linewidth=2)
    font1 = {'size': 18}
    plt.legend(loc=4, prop=font1)
    plt.xlabel(u'Iteration', size=24)
    plt.ylabel(u'Loss', size=24)
    plt.show()
    saver.save(sess, "save1/model.ckpt")

