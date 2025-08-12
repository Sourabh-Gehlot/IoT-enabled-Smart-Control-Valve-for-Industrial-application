import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn.preprocessing import LabelEncoder
from sklearn.metrics import accuracy_score, classification_report

# Load dataset
df = pd.read_csv("synthetic_sensor_data.csv")

# Encode target labels (ON/OFF)
le = LabelEncoder()
df['Relay_Status_Encoded'] = le.fit_transform(df['Relay_Status'])  # ON = 1, OFF = 0

# Define features and target
X = df[["Temperature_C", "FlowRate_Lmin", "Accel_X", "Accel_Y", "Accel_Z"]]
y = df["Relay_Status_Encoded"]

# Split into training and testing sets (80-20 split)
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# Train a Random Forest classifier
model = RandomForestClassifier(n_estimators=100, random_state=42)
model.fit(X_train, y_train)

# Predictions
y_pred = model.predict(X_test)

# Evaluation
print("Accuracy:", accuracy_score(y_test, y_pred))
print("\nClassification Report:\n", classification_report(y_test, y_pred, target_names=le.classes_))
