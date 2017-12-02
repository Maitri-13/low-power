package com.example.maitrichattopadhyay.lowpower;

import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import com.google.firebase.iid.FirebaseInstanceId;

import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.Socket;
import java.net.URL;


public class MainActivity extends AppCompatActivity {
    int count;
    MediaPlayer song;
    Button play, stop;
    private boolean playPause;
    private MediaPlayer mediaPlayer;
    private ProgressDialog progressDialog;
    private boolean initialStage = true;


    private class MyBroadcastReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            Log.i("Broadcast Receiver", "about to call onReceive");
            Bundle extras = intent.getExtras();
            String state = extras.getString("extra");
            TextView text = (TextView) findViewById(R.id.textView2);
            text.setText(state);// update your textView in the main layout

        }
    }



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        Button info = (Button) findViewById(R.id.info);
        Button history = (Button) findViewById(R.id.history);

        final Intent infoIntent = new Intent(this, Main2Activity.class);
        info.setOnClickListener(new View.OnClickListener() {
                                    @Override
                                    public void onClick(View v) {
                                        startActivity(infoIntent);
                                    }
                                });
        final Intent historyIntent = new Intent(this, Main3Activity.class);

        history.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                class NewThread extends AsyncTask<String, Void, String > {

                    private Exception exception;

                    protected String doInBackground(String... urls)
                    {
                        try {
                            // Creating new socket connection to the IP (first parameter) and its opened port (second parameter)
                            Socket s = new Socket("52.25.91.54", 5566);

                            // Initialize output stream to write message to the socket stream
                            BufferedWriter out = new BufferedWriter(new OutputStreamWriter(s.getOutputStream()));
                            BufferedReader in = new BufferedReader(new InputStreamReader(s.getInputStream()));
                            String outMsg = "DATA REQUEST";
                            // Write message to stream
                            out.write(outMsg);

                            // Flush the data from the stream to indicate end of message
                            out.flush();

                            String strTcp = in.readLine().toString();
                            // Close the output stream
                            out.close();
                            in.close();
                            // Close the socket connection
                            s.close();
                            JSONObject my_json = null;
                                my_json = new JSONObject(strTcp);

                            return strTcp;
                        }
                        catch (Exception e) {
                            this.exception = e;
                            Log.i("the exception", e.toString());
                            return null;
                        } finally {
                            Log.i("inside the finally block", "a");
                        }
                    }

                    protected void onPostExecute(String onUsed)
                    {
                        historyIntent.putExtra("json", onUsed);
                    String newText = historyIntent.getStringExtra("json");
                        Log.i("print!!!!!!1", newText );
                        startActivity(historyIntent);
                    }
                } // class
                String s = "stupid";
                new NewThread().execute(s);



            }

        });

        String my_token;
        my_token = FirebaseInstanceId.getInstance().getToken();


        Log.i("my new token ",my_token);


        TextView token = (TextView) findViewById(R.id.textView2);
        token.setText("");

        count = 0;
        final ImageView cameraImage = (ImageView) findViewById(R.id.cameraImage);
        Button imageChange = (Button) findViewById(R.id.imageChangerButton);
        imageChange.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view) {

                    class GetPicture extends AsyncTask<String, Void, Bitmap > {

                        protected Bitmap doInBackground(String... urls) {

                            try
                            {
                               URL url = new URL(urls[0]);
                                Bitmap bmp = BitmapFactory.decodeStream(url.openConnection().getInputStream());
                                return bmp;
                            } catch (Exception e)

                            {
                                Log.i("excep", e.toString());
                                return null;
                            }

                        }
                        protected void onPostExecute(Bitmap onUsed)
                        {
                            cameraImage.setImageBitmap(onUsed);

                        }
                    }

                          String url = "https://images.pexels.com/photos/36753/flower-purple-lical-blosso.jpg";
                           new GetPicture().execute(url);

                    }

//                if (count == 0) {
//                    cameraImage.setImageDrawable(getResources().getDrawable(R.drawable.h3));
//                    count++;
//                } else if (count == 1) {
//                    cameraImage.setImageDrawable(getResources().getDrawable(R.drawable.h));
//                    count++;
//                } else {
//                    cameraImage.setImageDrawable(getResources().getDrawable(R.drawable.hqdefault));
//                    count = 0;
//                }

        });


        play = (Button) findViewById(R.id.play);

        final String url = "https://www.mfiles.co.uk/mp3-downloads/Toccata-and-Fugue-Dm.mp3";

        mediaPlayer = new MediaPlayer();
        mediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
        progressDialog = new ProgressDialog(this);
        play.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (!playPause) {
                    play.setText("Pause Streaming");

                    if (initialStage) {
                        new Player().execute(url);
                    } else {
                        if (!mediaPlayer.isPlaying())
                            mediaPlayer.start();
                    }

                    playPause = true;

                } else {
                    play.setText("Launch Streaming");

                    if (mediaPlayer.isPlaying()) {
                        mediaPlayer.pause();
                    }

                    playPause = false;
                }
            }
        });
    }

    @Override
    protected void onPause() {
        super.onPause();

        if (mediaPlayer != null) {
            mediaPlayer.reset();
            mediaPlayer.release();
            mediaPlayer = null;
        }
    }

    class Player extends AsyncTask<String, Void, Boolean> {
        @Override
        protected Boolean doInBackground(String... strings) {
            Boolean prepared = false;

            try {
                mediaPlayer.setDataSource(strings[0]);
                mediaPlayer.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
                    @Override
                    public void onCompletion(MediaPlayer mediaPlayer) {
                        initialStage = true;
                        playPause = false;
                        play.setText("Launch Streaming");
                        mediaPlayer.stop();
                        mediaPlayer.reset();
                    }
                });

                mediaPlayer.prepare();
                prepared = true;

            } catch (Exception e) {
                Log.e("MyAudioStreamingApp", e.getMessage());
                prepared = false;
            }

            return prepared;
        }


        @Override
        protected void onPostExecute(Boolean aBoolean) {
            super.onPostExecute(aBoolean);

            if (progressDialog.isShowing()) {
                progressDialog.cancel();
            }

            mediaPlayer.start();
            initialStage = false;
        }

        @Override
        protected void onPreExecute() {
            super.onPreExecute();

            progressDialog.setMessage("Buffering...");
            progressDialog.show();
        }
    }


    @Override
    protected void onResume() {
        super.onResume();
        Log.i("Main App", "about to Resume");
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction("com.my.app.onMessageReceived");
        MyBroadcastReceiver receiver = new MyBroadcastReceiver();
        registerReceiver(receiver, intentFilter);


    }



}


